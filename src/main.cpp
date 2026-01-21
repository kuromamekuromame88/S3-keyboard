#include <Arduino.h>
#include <USB.h>
#include <USBHIDGamepad.h>

USBHIDGamepad Gamepad;

void setup() {
  Gamepad.begin();
  USB.begin();
  delay(1000);

  // Wake-up press (X button, button 3)
  Gamepad.send(0, 0, 0, 0, 0, 0, 0, 1 << 2); // Button 3 (bit 2)
  delay(500);
  Gamepad.send(0, 0, 0, 0, 0, 0, 0, 0);
  
  // Three confirmation presses
  /*for (int i = 0; i < 3; i++) {
    Gamepad.send(0, 0, 0, 0, 0, 0, 0, 1 << 2);
    delay(500);
    Gamepad.send(0, 0, 0, 0, 0, 0, 0, 0);
    delay(500);
  }*/
}

void loop() {
  Gamepad.rightStick(map(analogRead(15), 0, 4095, -128, 127), map(analogRead(16)), 0, 4095, -128, 127);
  /*
  if (command.startsWith("PRESS_BUTTON_")) {
    String buttonStr = command.substring(13); // Extract number after "PRESS_BUTTON_"
    int buttonNum = buttonStr.toInt();
    if (buttonNum >= 0 && buttonNum <= 20) {
      Gamepad.send(0, 0, 0, 0, 0, 0, 0, 1 << (buttonNum - 1)); // Button N = bit (N-1)
      delay(500);
      Gamepad.send(0, 0, 0, 0, 0, 0, 0, 0);
    } else {
      Serial.println("Invalid button number (must be 1-20)");
    }
  }*/
}
