#include "TempMesure.h"
#include "Arduino.h"

TempMesure::TempMesure(int _tempSensorPin) {
  tempSensorPin = _tempSensorPin;
  Serial.println("TEMP SENSOR PIN: " + String(tempSensorPin));
  oneWire = OneWire(tempSensorPin);
  ds = DallasTemperature(&oneWire);
}

void TempMesure::begin() {
  ds.begin();
  ds.setResolution(12);
}

float TempMesure::mesureWaterTemp() {
  ds.requestTemperatures();
  float temperature = ds.getTempCByIndex(0);

  return temperature;
}
