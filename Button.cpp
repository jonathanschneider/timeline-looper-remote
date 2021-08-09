#include "Arduino.h"
#include "Button.h"

const long debounceDelay = 50;

Button::Button(int _buttonPin, int _ledPin)
{
  buttonPin = _buttonPin;
  ledPin = _ledPin;
  buttonState = false;
  lastButtonState = false;
  risingEdge = false;
  active = false;
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  lastDebounceTime = 0;
}

void Button::read()
{
  debounce();

  risingEdge = buttonState == HIGH && lastButtonState == LOW;
  lastButtonState = buttonState;
}

void Button::debounce() {
  bool reading = digitalRead(buttonPin);
  bool lastReading;

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonState = reading;
  }

  lastReading = reading;
}

void Button::updateLed() {
  digitalWrite(ledPin, active);
}
