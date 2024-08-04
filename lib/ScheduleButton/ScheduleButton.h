#include <ADebouncer.h>
#include <ESPDateTime.h>

class ScheduleButton {
 public:
  bool state;
  ADebouncer debouncer;
  int buttonPin;
  int ledGreenPin;
  int ledRedPin;
  int scheduleOnStartTime;

 public:
  ScheduleButton(int _buttonPin, int _ledGreenPin, int _ledRedPin);
  bool checkButton();
  bool getState();
  void setState(bool _state);
  void toggleState();
  int getScheduleOnStartTime();
};
