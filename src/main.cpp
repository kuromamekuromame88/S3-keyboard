#include <USB.h>
#include <USBHIDGamepad.h>

USBHIDGamepad Gamepad;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32-S2 Starting...");
  Gamepad.begin();
  USB.begin();
  delay(1000);

  // Wake-up press (X button, button 3)
  Serial.println("Waking up device with X button press...");
  Gamepad.send(0, 0, 0, 0, 0, 0, 0, 1 << 2); // Button 3 (bit 2)
  delay(500);
  Gamepad.send(0, 0, 0, 0, 0, 0, 0, 0);
  Serial.println("Wake-up X button released");

  // Three confirmation presses
  for (int i = 0; i < 3; i++) {
    Serial.println("Confirmation X button press...");
    Gamepad.send(0, 0, 0, 0, 0, 0, 0, 1 << 2);
    delay(500);
    Gamepad.send(0, 0, 0, 0, 0, 0, 0, 0);
    delay(500);
    Serial.println("Confirmation X button released");
  }
  Serial.println("INIT_COMPLETE");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    Serial.print("Received: '");
    Serial.print(command);
    Serial.println("'");

    // Check for PRESS_BUTTON_N commands (N = 1 to 20)
    if (command.startsWith("PRESS_BUTTON_")) {
      String buttonStr = command.substring(13); // Extract number after "PRESS_BUTTON_"
      int buttonNum = buttonStr.toInt();
      if (buttonNum >= 0 && buttonNum <= 20) {
        Serial.print("Pressing button ");
        Serial.print(buttonNum);
        Serial.println("...");
        Gamepad.send(0, 0, 0, 0, 0, 0, 0, 1 << (buttonNum - 1)); // Button N = bit (N-1)
        delay(500);
        Gamepad.send(0, 0, 0, 0, 0, 0, 0, 0);
        Serial.print("Button ");
        Serial.print(buttonNum);
        Serial.println(" released");
      } else {
        Serial.println("Invalid button number (must be 1-20)");
      }
    } else {
      Serial.println("Unknown command");
    }
  }
}
