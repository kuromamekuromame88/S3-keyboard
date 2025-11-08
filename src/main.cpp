#include <Arduino.h>
#include "driver/i2s.h"
#include <math.h>

// ===== I2S設定 =====
#define I2S_DOUT 5
#define I2S_BCLK 6
#define I2S_LRC  4

#define SAMPLE_RATE 44100
#define LUT_SIZE 256

// ===== I2S初期化 =====
void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
}

// ===== サイン波テーブル =====
int16_t sineLUT[LUT_SIZE];
void initSineLUT() {
  for (int i = 0; i < LUT_SIZE; i++) {
    sineLUT[i] = (int16_t)(sin(2 * PI * i / LUT_SIZE) * 3000);
  }
}

// ===== 音名 → 周波数（半音対応） =====
float noteToFrequency(String note) {
  note.trim();
  if (note.length() < 2) return 0;
  int octave = note[note.length() - 1] - '0';
  String n = note.substring(0, note.length() - 1);
  float base = 0;

  if (n == "C") base = 16.35;
  else if (n == "C#") base = 17.32;
  else if (n == "D") base = 18.35;
  else if (n == "D#") base = 19.45;
  else if (n == "E") base = 20.60;
  else if (n == "F") base = 21.83;
  else if (n == "F#") base = 23.12;
  else if (n == "G") base = 24.50;
  else if (n == "G#") base = 25.96;
  else if (n == "A") base = 27.50;
  else if (n == "A#") base = 29.14;
  else if (n == "B") base = 30.87;
  else return 0;

  return base * pow(2, octave);
}

// ===== 音を鳴らす =====
void playTone(float freq, int durationMs) {
  if (freq <= 0) {
    delay(durationMs);
    return;
  }

  size_t written;
  int16_t buffer[256];
  float phaseIncrement = freq * LUT_SIZE / SAMPLE_RATE;
  float phase = 0;
  int totalSamples = SAMPLE_RATE * durationMs / 1000;

  for (int i = 0; i < totalSamples; i += 256) {
    for (int j = 0; j < 256; j++) {
      buffer[j] = sineLUT[(int)phase % LUT_SIZE];
      phase += phaseIncrement;
    }
    i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &written, portMAX_DELAY);
  }
}

// ===== 音符長 → 時間(ms) =====
int lengthToMs(int length) {
  int base = 400; // 四分音符 = 400ms
  return base * (4.0 / length);
}

// ===== 曲を再生 =====
void playMelody(String melody) {
  melody += ",";
  int lastPos = 0;
  while (true) {
    int comma = melody.indexOf(",", lastPos);
    if (comma == -1) break;
    String token = melody.substring(lastPos, comma);
    lastPos = comma + 1;

    token.trim();
    if (token.length() == 0) continue;

    int colon = token.indexOf(":");
    String note = token.substring(0, colon);
    int length = (colon != -1) ? token.substring(colon + 1).toInt() : 4;

    float freq = noteToFrequency(note);
    int duration = lengthToMs(length);

    playTone(freq, duration);
    delay(10);
  }
}

// ===== セットアップ =====
void setup() {
  Serial.begin(115200);
  setupI2S();
  initSineLUT();
  Serial.println("PCM5102 Melody Player Ready!");

  // 🎶 規定メロディ再生
  String defaultMelody = "C4:4,D4:4,E4:4,F4:4,G4:8";
  playMelody(defaultMelody);

  Serial.println("規定メロディ再生完了。シリアルで曲データを入力してください。");
  Serial.println("--------音データ---------");
  Serial.println("C4:4,C#4:4,D4:4,D#4:4,E4:4,F4:4,F#4:4,G4:4,G#4:4,A4:4,A#4:4,B4:4");
  Serial.println("------------------------");
}

// ===== ループでシリアル入力をチェック =====
String serialBuffer = "";
void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      serialBuffer.trim();
      if (serialBuffer.length() > 0) {
        Serial.println("曲データ受信: " + serialBuffer);
        playMelody(serialBuffer);
      }
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }
}
