class Button
{
public:
  Button(int _midi, int _buttonPin, int _ledPin);
  int midi;
  int buttonPin;
  int led;
  bool button;
  bool buttonLong;
  bool state;
  bool slope;
  bool reading;
  bool lastButtonState;
  long lastDebounceTime;
  void debounce();

};



