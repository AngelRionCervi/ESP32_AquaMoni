#include "SensorTask.h"

void SensorTaskCode(void* pvParameters) {
  Serial.print("SensorTask running on core ");
  Serial.println(xPortGetCoreID());

  PhMesure phMesure(PH_SENSOR_PIN, PH_NEUTRAL_VOLTAGE, PH_ACID_VOLTAGE);
  TempMesure tempMesure(TEMP_SENSOR_PIN);

  for (;;) {
    float phValue = phMesure.mesurePh();
    float tempValue = tempMesure.mesureWaterTemp();

    if (phValue > 15 || tempValue < 0) {
      sensorError = true;
    }

    float ph = decimalRound(phValue, 2);
    float temp = decimalRound(tempValue, 2);

    Serial.println("temp: " + String(temp));
    Serial.println("ph: " + String(ph));

    // writeToSd(ph, temp);

    // delay(10*60*1000);

    delay(5000);
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
  }
}