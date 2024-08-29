#include "BtSetup.h"

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Device connected");
    btDeviceConnected = true;
    BLEDevice::startAdvertising();
  };

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Device disconnected");
    btDeviceConnected = false;
  }
};

class CharacCallbacks : public BLECharacteristicCallbacks {
 public:
  String name;
  CharacCallbacks(String name) : name(name) {}

  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      Serial.println(name);
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*********");
    }
  }
};

void bt_begin() {
  Serial.println("Entering bluetooth setup");
  BLEDevice::init("Aqua Pal");
  BLEServer* pServer = BLEDevice::createServer();

  pServer->setCallbacks(new ServerCallbacks());
  BLEService* pService = pServer->createService(BT_SERVICE_UUID);

  BLEDescriptor btSSIDDescriptor(BT_SSID_DESCRIPTOR_UUID);
  BLEDescriptor btWifiPassDescriptor(BT_WIFIPASS_DESCRIPTOR_UUID);
  BLEDescriptor btConfigDoneDescriptor(BT_CONFIG_DONE_DESCRIPTOR_UUID);

  btSSIDDescriptor.setValue("network_ssid");
  btWifiPassDescriptor.setValue("network_password");
  btConfigDoneDescriptor.setValue("config_done");

  BLECharacteristic btSSIDCharacteristic(
      BT_SSID_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  BLECharacteristic btWifiPassCharacteristic(
      BT_WIFIPASS_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  BLECharacteristic btConfigDoneCharacteristic(
      BT_CONFIG_DONE_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pService->addCharacteristic(&btSSIDCharacteristic);
  btSSIDCharacteristic.addDescriptor(&btSSIDDescriptor);
  btSSIDCharacteristic.setValue("null");

  pService->addCharacteristic(&btWifiPassCharacteristic);
  btWifiPassCharacteristic.addDescriptor(&btWifiPassDescriptor);
  btWifiPassCharacteristic.setValue("null");

  pService->addCharacteristic(&btConfigDoneCharacteristic);
  btConfigDoneCharacteristic.addDescriptor(&btConfigDoneDescriptor);
  btConfigDoneCharacteristic.setValue("false");

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BT_SERVICE_UUID);

  pAdvertising->setScanResponse(false);
  //  pAdvertising->setMinPreferred(
  //      0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  btWifiPassCharacteristic.setCallbacks(
      new CharacCallbacks(BT_SSID_CHARACTERISTIC_NAME));
  btSSIDCharacteristic.setCallbacks(
      new CharacCallbacks(BT_WIFIPASS_CHARACTERISTIC_NAME));
  btConfigDoneCharacteristic.setCallbacks(
      new CharacCallbacks(BT_CONFIG_DONE_CHARACTERISTIC_NAME));

  Serial.println("Characteristic defined! Now you can read it in your phone!");

  isInBtSetup = true;

  for (;;) {
    yield();
  }
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
  // if (SerialBT.available()) {
  //   String command = SerialBT.readStringUntil('\n');
  //   command.trim();
  //   Serial.println(command);

  //   if (command.startsWith(BLUETOOTH_SETUP_SSID_CMD)) {
  //     String ssid = command.substring(strlen(BLUETOOTH_SETUP_SSID_CMD));
  //     wifiSSID = ssid;
  //     Serial.println("SSID set to: " + wifiSSID);
  //   } else if (command.startsWith(BLUETOOTH_SETUP_WIFIPASS_CMD)) {
  //     String pass = command.substring(strlen(BLUETOOTH_SETUP_WIFIPASS_CMD));
  //     wifiPass = pass;
  //     Serial.println("Wifi password set to: " + wifiPass);
  //   } else if (command.equals(BLUETOOTH_SETUP_VALIDATE_CMD)) {
  //     bool isSaved = bt_saveCredToSD();
  //     if (isSaved) {
  //       Serial.println("WiFi credentials saved to SD card");
  //       Serial.println("Bluetooth setup complete, restarting...");
  //       ESP.restart();
  //     } else {
  //       Serial.println("Failed to save WiFi credentials to SD card");
  //     }
  //   } else {
  //     Serial.println("Unknown command: " + command);
  //   }
  // }
}
