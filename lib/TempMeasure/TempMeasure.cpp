#include "TempMeasure.h"
#include "Arduino.h"

TempMeasure::TempMeasure(int _tempSensorPin) {
  tempSensorPin = _tempSensorPin;
  oneWire = OneWire(tempSensorPin);
  ds = DallasTemperature(&oneWire);
}

void TempMeasure::begin() {
  ds.begin();
  ds.setResolution(12);
}

float TempMeasure::measureWaterTemp() {
  ds.requestTemperatures();
  float temperature = ds.getTempCByIndex(0);

  return temperature;
}
