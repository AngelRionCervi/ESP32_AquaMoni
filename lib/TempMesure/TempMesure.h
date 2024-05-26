#include <OneWire.h>
#include <DallasTemperature.h>

class TempMesure {
public:
  OneWire oneWire;
  DallasTemperature ds;
  int tempSensorPin;
public:
  TempMesure(int tempSensorPin);
  void begin();
  float mesureWaterTemp();
};
