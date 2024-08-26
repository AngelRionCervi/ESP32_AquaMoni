#include "BtSetup.h"

void bt_begin() {
  isInBtSetup = true;
  SerialBT.begin(BLUETOOTH_NAME);
  Serial.println("Entering bluetooth setup");
}

bool bt_saveCredToSD() {
  File fileConfigRead = SD.open(FILE_CONFIG, "r");

  if (!fileConfigRead) {
    Serial.println("Could not open config.jso [bt_saveCredToSD]");
    activityLed.setState("error");
    activityLed.update();

    return false;
  }

  String configString;
  while (fileConfigRead.available()) {
    configString += fileConfigRead.readString();
  }
  fileConfigRead.close();

  File fileConfigWrite = SD.open(FILE_CONFIG, "w");

  JsonDocument configJson;
  deserializeJson(configJson, configString);

  configJson["secrets"]["wifiSSID"] = wifiSSID;
  configJson["secrets"]["wifiPass"] = wifiPass;

  serializeJson(configJson, fileConfigWrite);
  fileConfigWrite.close();

  return true;
}

void bt_readSerial() {
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim();
    Serial.println(command);

    if (command.startsWith(BLUETOOTH_SETUP_SSID_CMD)) {
      String ssid = command.substring(strlen(BLUETOOTH_SETUP_SSID_CMD));
      wifiSSID = ssid;
      Serial.println("SSID set to: " + wifiSSID);
    } else if (command.startsWith(BLUETOOTH_SETUP_WIFIPASS_CMD)) {
      String pass = command.substring(strlen(BLUETOOTH_SETUP_WIFIPASS_CMD));
      wifiPass = pass;
      Serial.println("Wifi password set to: " + wifiPass);
    } else if (command.equals(BLUETOOTH_SETUP_VALIDATE_CMD)) {
      bool isSaved = bt_saveCredToSD();
      if (isSaved) {
        Serial.println("WiFi credentials saved to SD card");
        Serial.println("Bluetooth setup complete, restarting...");
        ESP.restart();
      } else {
        Serial.println("Failed to save WiFi credentials to SD card");
      }
    } else {
      Serial.println("Unknown command: " + command);
    }
  }
}
