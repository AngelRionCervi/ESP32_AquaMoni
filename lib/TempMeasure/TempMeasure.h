#include <OneWire.h>
#include <DallasTemperature.h>

class TempMeasure {
public:
  OneWire oneWire;
  DallasTemperature ds;
  int tempSensorPin;
public:
  TempMeasure(int _tempSensorPin);
  void begin();
  float measureWaterTemp();
};
