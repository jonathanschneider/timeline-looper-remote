#include "Arduino.h"
#include "Button.h"

const unsigned short DEBOUNCE_DELAY = 50;
const unsigned short LONG_PRESS = 1000;

Button::Button(int _buttonPin, int _ledPin)
{
  buttonPin = _buttonPin;
  ledPin = _ledPin;
  buttonState = false;
  lastButtonState = false;
  risingEdge = false;
  longPressActive = false;
  longPressReleased = false;
  active = false;
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  debounceTimer = 0;
  pressTimer = 0;
}

void Button::read()
{
  buttonState = debounce();

  if (buttonState == true) {
    if (lastButtonState == false) {
      pressTimer = millis();
    }

    // Long press
    if ((millis() - pressTimer > LONG_PRESS) && (longPressActive == false)) {
  		longPressActive = true;
  	}
  }
  else if (longPressActive == true) {
    longPressReleased = true;
    longPressActive = false;
  }

  risingEdge = buttonState == true && lastButtonState == false;

  lastButtonState = buttonState;
}

bool Button::debounce() {
  bool reading = digitalRead(buttonPin);
  bool lastReading;

  if (reading != lastReading) {
    debounceTimer = millis();
  }

  if ((millis() - debounceTimer) > DEBOUNCE_DELAY) {
    return reading;
  }

  lastReading = reading;
}

void Button::updateLed() {
  digitalWrite(ledPin, active);
}
