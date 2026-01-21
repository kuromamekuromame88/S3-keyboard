#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad BG;

// ===== ピン定義 =====
#define MStart1 42

#define LX 18
#define LY 17
#define RX 15
#define RY 16

#define LB 39
#define RB 40

#define UP 3
#define RIGHT 9
#define DOWN 10
#define LEFT 11

#define ZL 35
#define ZR 36
#define L 2
#define R 1

#define PLUS 38
#define MINUS 37

#define BTN_B 5
#define BTN_A 4
#define BTN_X 7
#define BTN_Y 6

int16_t readAxe(int pin){
  return map(analogRead(pin), 0, 4095, -32768, 32767);
}

void setup() {
  BG.begin();
}

void loop() {
  if (bleGamepad.isConnected())
    {
        /*
        bleGamepad.press(BUTTON_5);
        bleGamepad.press(BUTTON_16);
        bleGamepad.pressStart();
        bleGamepad.setAxes(readAxe(LX), 32767, 32767, 32767, 32767, 32767, 32767, 32767);       //(X, Y, Z, RX, RY, RZ)
        //bleGamepad.setHIDAxes(32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767);  //(X, Y, Z, RZ, RX, RY)
        bleGamepad.setHat1(HAT_DOWN_RIGHT);
        // All axes, sliders, hats etc can also be set independently. See the IndividualAxes.ino example
        delay(500);

        Serial.println("Release button 5 and start. Move all axes to min. Set DPAD (hat 1) to centred.");
        bleGamepad.release(BUTTON_5);
        bleGamepad.releaseStart();
        bleGamepad.setHat1(HAT_CENTERED);
        bleGamepad.setAxes(0, 0, 0, 0, 0, 0, 0, 0);           //(X, Y, Z, RX, RY, RZ)
        //bleGamepad.setHIDAxes(0, 0, 0, 0, 0, 0, 0, 0);      //(X, Y, Z, RZ, RX, RY)
        delay(500);
        */
        bleGamepad.setAxes(readAxe(LX), 32767, 32767, 32767, 32767, 32767, 32767, 32767);       //(X, Y, Z, RX, RY, RZ)
        delay(5);
    }
}
