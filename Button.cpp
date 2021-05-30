#include "Arduino.h"
#include "Button.h"

long debounceDelay = 50;

Button::Button(int _midi, int _buttonPin, int _ledPin)
{
  midi = _midi;
  buttonPin = _buttonPin;
  led = _ledPin;
  button = false;
  state = false;
  slope = false;
  pinMode(buttonPin, INPUT);
  pinMode(led, OUTPUT);
  lastDebounceTime = 0;
}

void Button::debounce()
{
  bool reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  } 

  if ((millis() - lastDebounceTime) > debounceDelay)
    button = reading;

  lastButtonState = reading;
}
