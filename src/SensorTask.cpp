#include "SensorTask.h"

void SensorTaskCode(void* pvParameters) {
  Serial.print("SensorTask running on core ");
  Serial.println(xPortGetCoreID());

  PhMeasure phMeasure(PH_SENSOR_PIN, PH_NEUTRAL_VOLTAGE, PH_ACID_VOLTAGE);
  TempMeasure tempMeasure(TEMP_SENSOR_PIN);

  for (;;) {
    int millisNow = millis();

    if ((millisNow - measurementsUpdateLastMillis >
         measurementsUpdatePeriode) &&
        enableMonitoring) {
      takeMeasurements(phMeasure, tempMeasure, true);
      measurementsUpdateLastMillis = millisNow;
    }

    if (millisNow - liveMeasurementsLastMillis >
        updateLiveMeasurementsPeriode) {
      takeMeasurements(phMeasure, tempMeasure, false);
      liveMeasurementsLastMillis = millisNow;
    }

    delay(2);
  }
}

void takeMeasurements(PhMeasure& phMeasure,
                      TempMeasure& tempMeasure,
                      bool saveToSd) {
  float phValue = phMeasure.mesurePh();
  float tempValue = tempMeasure.measureWaterTemp();
  phCalibrationMv = phMeasure.getCalibrationVoltage();

  if (phValue > 15 || tempValue < 0) {
    sensorError = true;
    activityLed.setState("errorBlink");
  } else {
    sensorError = false;
    activityLed.setState("ok");
  }

  float ph = decimalRound(phValue, 2);
  float temp = decimalRound(tempValue, 2);

  monitoringLiveMap["ph"] = ph;
  monitoringLiveMap["temp"] = temp;

  if (saveToSd) {
    writeToSd(ph, temp);
  }
}

void writeToSd(float ph, float temp) {
  JsonDocument currentData;
  currentData["t"] = temp;
  currentData["p"] = ph;
  currentData["d"] = DateTime.now();

  writeToLast(&currentData);
  writeToHistorical(&currentData);
}

void writeToLast(JsonDocument* currentData) {
  File fileLast = SD.open(FILE_LAST, "w");

  if (fileLast) {
    serializeJson(*currentData, fileLast);
    fileLast.close();
    Serial.println("updated last.jso");
  } else {
    Serial.println("error writing last.jso");
    activityLed.setState("errorBlink");
  }
}

void writeToHistorical(JsonDocument* currentData) {
  if (!SD.exists("/historical")) {
    SD.mkdir("/historical");
  }

  DateTimeParts dateParts = DateTime.getParts();

  String currentDay = "";
  currentDay += dateParts.getMonthDay();
  currentDay += "_";
  currentDay += (dateParts.getMonth() + 1);

  String historicalPath = "";
  historicalPath += "/historical/";
  historicalPath += currentDay;
  historicalPath += ".jso";

  File fileHistorical = SD.open(historicalPath, "a");

  if (fileHistorical) {
    serializeJson(*currentData, fileHistorical);
    fileHistorical.println();
    fileHistorical.close();
    Serial.println("updated historical.jso");
  } else {
    Serial.println("error writing historical.jso");
    activityLed.setState("errorBlink");
  }
}
