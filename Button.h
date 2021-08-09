class Button
{
public:
  Button(int _buttonPin, int _ledPin);
  unsigned short buttonPin;
  unsigned short ledPin;
  bool buttonState;
  bool lastButtonState;
  bool risingEdge;
  bool active;
  void read();
  void updateLed();
private:
  unsigned long lastDebounceTime;
  void debounce();
};
