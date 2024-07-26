#include <ADebouncer.h>
#include <ArduinoJson.h>
#include <ESPDateTime.h>
#include <ESPmDNS.h>
#include <SD.h>
#include <SPI.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include <map>
#include <unordered_map>

#include "Device.h"
#include "SensorTask.h"
#include "ServerTask.h"
#include "constants.h"
#include "global.h"
#include "secrets.h"

TaskHandle_t SensorTask;
TaskHandle_t ServerTask;

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPass);
  Serial.print("WiFi Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    activityLed.update();
    delay(50);
  }
  Serial.println();
}

void setupDateTime() {
  DateTime.setServer(NTP_SERVER_ADDRESS);
  DateTime.setTimeZone("UTC-2");
  DateTime.begin();
  activityLed.update();
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
    activityLed.setState("error");
    activityLed.update();
    delay(5000);
    setupDateTime();
  } else {
    activityLed.update();
    Serial.printf("Date Now is %s\n", DateTime.toISOString().c_str());
    Serial.printf("Timestamp is %ld\n", DateTime.now());
  }
}

void setupSD() {
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    activityLed.setState("error");
    activityLed.update();
    delay(5000);
    setupSD();
  }
}

JsonDocument getConfig() {
  File fileConfig = SD.open(FILE_CONFIG, "r");
  activityLed.update();

  if (!fileConfig) {
    Serial.println("Could not open config.jso [loadConfig]");
    activityLed.setState("error");
    activityLed.update();
  }

  String configString;
  while (fileConfig.available()) {
    configString += fileConfig.readString();
  }

  JsonDocument configJson;
  deserializeJson(configJson, configString);

  return configJson;
}

void loadConfig(JsonDocument& configJson) {
  activityLed.update();
  JsonObject secretsJson = configJson["secrets"];
  wifiSSID = secretsJson["wifiSSID"].as<String>();
  wifiPass = secretsJson["wifiPass"].as<String>();
  serverPass = secretsJson["serverPass"].as<String>();
}

void setupDevices(JsonDocument& configJson) {
  JsonArray deviceArray = configJson["devices"];

  for (int i = 0; i < deviceArray.size(); i++) {
    JsonObject deviceObj = deviceArray[i];
    const char* name = deviceObj["name"].as<const char*>();
    const char* ip = deviceObj["ip"].as<const char*>();
    const char* id = deviceObj["id"].as<const char*>();
    unsigned int button = deviceObj["button"].as<unsigned int>();
    JsonVariant schedule = deviceObj["schedule"].as<JsonVariant>();

    JsonDocument scheduleCopy = schedule;

    Device newDevice(ip, name, id, ledMap[button], buttonMap[button], button,
                     scheduleCopy, wifiClient);
    devices.emplace(id, newDevice);
    activityLed.update();
  }
}

void setup(void) {
  Serial.begin(115200);
  activityLed.setState("okBlink");
  activityLed.update();

  while (!Serial) {  // to remove for prod
    activityLed.update();
    delay(50);
  };

  sessionId = "hola";  // getRandomString(16);

  setupSD();
  JsonDocument config = getConfig();
  loadConfig(config);
  setupWifi();
  setupDateTime();

  Serial.println("Setting up devices...");
  setupDevices(config);

  scheduleButton.update(false);

  activityLed.setState("ok");

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
      1);             /* pin task to core 1 */
}

void loop(void) {
  // server.handleClient();
  delay(2);  // allow the cpu to switch to other tasks
}