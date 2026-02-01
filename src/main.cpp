#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "Adafruit_TinyUSB.h"

// =====================
// WiFi AP 設定
// =====================
const char* AP_SSID = "ESP32S3-Keyboard";
const char* AP_PASS = "12345678";

// =====================
// USB HID
// =====================
Adafruit_USBD_HID usb_hid;

// 6キー同時押し用バッファ
uint8_t keycodes[6] = {0, 0, 0, 0, 0, 0};

// =====================
// Web Server
// =====================
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// =====================
// JS code → HID Keycode（US配列）
// =====================
uint8_t keycodeFromJS(const String& code) {
  if (code.startsWith("Key"))
    return HID_KEY_A + (code[3] - 'A');

  if (code.startsWith("Digit")) {
    if (code[5] == '0') return HID_KEY_0;
    return HID_KEY_1 + (code[5] - '1');
  }

  if (code == "Space") return HID_KEY_SPACE;
  if (code == "Enter") return HID_KEY_ENTER;
  if (code == "Backspace") return HID_KEY_BACKSPACE;
  if (code == "Tab") return HID_KEY_TAB;
  if (code == "Escape") return HID_KEY_ESCAPE;

  if (code == "Minus") return HID_KEY_MINUS;
  if (code == "Equal") return HID_KEY_EQUAL;
  if (code == "BracketLeft") return HID_KEY_BRACKET_LEFT;
  if (code == "BracketRight") return HID_KEY_BRACKET_RIGHT;
  if (code == "Backslash") return HID_KEY_BACKSLASH;
  if (code == "Semicolon") return HID_KEY_SEMICOLON;
  if (code == "Quote") return HID_KEY_APOSTROPHE;
  if (code == "Comma") return HID_KEY_COMMA;
  if (code == "Period") return HID_KEY_PERIOD;
  if (code == "Slash") return HID_KEY_SLASH;
  if (code == "Backquote") return HID_KEY_GRAVE;

  return 0;
}

// =====================
// WebSocket handler
// =====================
void onWsEvent(AsyncWebSocket* server,
               AsyncWebSocketClient* client,
               AwsEventType type,
               void* arg,
               uint8_t* data,
               size_t len) {

  if (type != WS_EVT_DATA) return;

  JsonDocument doc;
  if (deserializeJson(doc, data, len)) return;

  String action = doc["type"];
  String code   = doc["code"];

  uint8_t key = keycodeFromJS(code);
  if (!key) return;

  if (action == "down") {
    uint8_t modifier = 0;
    if (doc["ctrl"])  modifier |= KEYBOARD_MODIFIER_LEFTCTRL;
    if (doc["shift"]) modifier |= KEYBOARD_MODIFIER_LEFTSHIFT;
    if (doc["alt"])   modifier |= KEYBOARD_MODIFIER_LEFTALT;
    if (doc["meta"])  modifier |= KEYBOARD_MODIFIER_LEFTGUI;

    // 単キーなので keycodes[0] に入れる
    keycodes[0] = key;

    usb_hid.keyboardReport(0, modifier, keycodes);
  }

  if (action == "up") {
    memset(keycodes, 0, sizeof(keycodes));
    usb_hid.keyboardRelease(0);
  }
}

// =====================
// setup
// =====================
void setup() {
  usb_hid.begin();

  WiFi.softAP(AP_SSID, AP_PASS);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head><meta charset="utf-8"></head>
<body>
<h2>ESP32-S3 Wireless Keyboard</h2>
<p>Click here and type</p>

<script>
const ws = new WebSocket(`ws://${location.host}/ws`);

function send(type, e) {
  ws.send(JSON.stringify({
    type: type,
    code: e.code,
    ctrl: e.ctrlKey,
    shift: e.shiftKey,
    alt: e.altKey,
    meta: e.metaKey
  }));
}

document.addEventListener("keydown", e => {
  e.preventDefault();
  send("down", e);
});

document.addEventListener("keyup", e => {
  e.preventDefault();
  send("up", e);
});
</script>
</body>
</html>
)rawliteral");
  });

  server.begin();
}

void loop() {}
