#include "BtSetup.h"

String tempSSID = "";
String tempWifiPass = "";

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
    String value = pCharacteristic->getValue().c_str();

    Serial.println("Received value: " + value + " for characteristic: " + name);

    if (value.length() > 0) {
      if (name == BT_SSID_CHARACTERISTIC_NAME) {
        tempSSID = value;
      } else if (name == BT_WIFIPASS_CHARACTERISTIC_NAME) {
        tempWifiPass = value;
      } else if (name == BT_CONFIG_DONE_CHARACTERISTIC_NAME) {
        if (value == "true") {
          bool isSaved = bt_saveCredToSD();
          if (isSaved) {
            Serial.println("WiFi credentials saved to SD card");
            ESP.restart();
          } else {
            Serial.println("Failed to save WiFi credentials to SD card");
          }
        }
      }
    }
  }
};

void bt_begin() {
  Serial.println("Entering bluetooth setup");
  BLEDevice::init(BT_DEVICE_NAME);
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
  btSSIDCharacteristic.setValue("");

  pService->addCharacteristic(&btWifiPassCharacteristic);
  btWifiPassCharacteristic.addDescriptor(&btWifiPassDescriptor);
  btWifiPassCharacteristic.setValue("");

  pService->addCharacteristic(&btConfigDoneCharacteristic);
  btConfigDoneCharacteristic.addDescriptor(&btConfigDoneDescriptor);
  btConfigDoneCharacteristic.setValue("false");

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BT_SERVICE_UUID);

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  BLEDevice::startAdvertising();

  btSSIDCharacteristic.setCallbacks(
      new CharacCallbacks(BT_SSID_CHARACTERISTIC_NAME));
  btWifiPassCharacteristic.setCallbacks(
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

  configJson["secrets"]["wifiSSID"] = tempSSID;
  configJson["secrets"]["wifiPass"] = tempWifiPass;

  serializeJson(configJson, fileConfigWrite);
  fileConfigWrite.close();

  return true;
}
