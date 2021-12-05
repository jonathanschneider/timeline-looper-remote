class Button
{
public:
  Button(int _buttonPin, int _ledPin);
  unsigned short buttonPin;
  unsigned short ledPin;
  bool buttonState;
  bool risingEdge;
  bool longPressActive;
  bool longPressReleased;
  bool active;
  void read();
  void updateLed();
private:
  bool lastButtonState;
  unsigned long debounceTimer;
  unsigned long pressTimer;
  bool debounce();
};
