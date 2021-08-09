class Button
{
public:
  Button(int _buttonPin, int _ledPin);
  int buttonPin;
  int ledPin;
  bool buttonState;
  bool lastButtonState;
  bool risingEdge;
  bool active;
  void read();
  void updateLed();
private:
  long lastDebounceTime;
  void debounce();
};
