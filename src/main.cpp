#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ADebouncer.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <ESPDateTime.h>
#include <unordered_map>
#include <map>
#include "secrets.h"
#include "constants.h"
#include "Device.h"
#include "global.h"
#include "ServerTask.h"
#include "SensorTask.h"

void(* resetFunc) (void) = 0;

TaskHandle_t SensorTask;
TaskHandle_t ServerTask;

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("WiFi Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println();
}

void setupDateTime() {
  DateTime.setServer(NTP_SERVER_ADDRESS);
  DateTime.setTimeZone("UTC-2");
  DateTime.begin();
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
    while (1)
      ;
  } else {
    Serial.printf("Date Now is %s\n", DateTime.toISOString().c_str());
    Serial.printf("Timestamp is %ld\n", DateTime.now());
  }
}

void setupSD() {
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    while (1)
      ;
  }
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {};

  setupSD();
  setupWifi();
  setupDateTime();

  Serial.println("Starting tasks...");

  xTaskCreatePinnedToCore(
    ServerTaskCode, /* Task function. */
    "ServerTask",   /* name of task. */
    10000,          /* Stack size of task */
    NULL,           /* parameter of the task */
    1,              /* priority of the task */
    &ServerTask,    /* Task handle to keep track of created task */
    0);             /* pin task to core 0 */

  xTaskCreatePinnedToCore(
    SensorTaskCode, /* Task function. */
    "SensorTask",   /* name of task. */
    10000,          /* Stack size of task */
    NULL,           /* parameter of the task */
    1,              /* priority of the task */
    &SensorTask,    /* Task handle to keep track of created task */
    1);             /* pin task to core 0 */
}

void loop(void) {
  // server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}