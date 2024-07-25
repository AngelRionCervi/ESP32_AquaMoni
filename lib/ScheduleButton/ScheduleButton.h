#include <ADebouncer.h>

class ScheduleButton {
 public:
  bool state;
  ADebouncer debouncer;
  int buttonPin;
  int ledGreenPin;
  int ledRedPin;

 public:
  ScheduleButton(int _buttonPin, int _ledGreenPin, int _ledRedPin);
  bool checkButton();
  void update(bool _state);
};
