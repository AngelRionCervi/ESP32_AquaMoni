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

  void onRead(BLECharacteristic* pCharacteristic) {
    Serial.println("Characteristic read: " + name);
  }
};

String bt_listWifiAccessPoints() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Wifi scan started");

  struct WifiNetwork {
    String ssid;
    String encryptionType;
    int rssi;
  };

  std::map<int, WifiNetwork> wifiNetworksMap;

  int totalNetworks = WiFi.scanNetworks();

  if (totalNetworks == 0) {
    Serial.println("No networks found");
  } else {
    // Serial.print(totalNetworks);
    // Serial.println(" networks found");
    // Serial.println(
    //     "Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < totalNetworks; ++i) {
      WifiNetwork wifiNetwork;
      String encryption;
      wifiNetwork.ssid = WiFi.SSID(i);

      wifiNetwork.rssi = WiFi.RSSI(i);

      // Print SSID and RSSI for each network found
      // Serial.printf("%2d", i + 1);
      // Serial.print(" | ");
      // Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      // Serial.print(" | ");
      // Serial.printf("%4d", WiFi.RSSI(i));
      // Serial.print(" | ");
      // Serial.printf("%2d", WiFi.channel(i));
      // Serial.print(" | ");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:
          encryption = "open";
          break;
        case WIFI_AUTH_WEP:
          encryption = "WEP";
          break;
        case WIFI_AUTH_WPA_PSK:
          encryption = "WPA";
          break;
        case WIFI_AUTH_WPA2_PSK:
          encryption = "WPA2";
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          encryption = "WPA+WPA2";
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          encryption = "WPA2-EAP";
          break;
        case WIFI_AUTH_WPA3_PSK:
          encryption = "WPA3";
          break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
          encryption = "WPA2+WPA3";
          break;
        case WIFI_AUTH_WAPI_PSK:
          encryption = "WAPI";
          break;
        default:
          encryption = "unknown";
      }
      wifiNetwork.encryptionType = encryption;
      wifiNetworksMap.emplace(i, wifiNetwork);
    }
  }
  Serial.println("");

  String wifiNetworksString = "";

  for (auto const& [_, network] : wifiNetworksMap) {
    wifiNetworksString +=
        network.ssid + "]" + network.encryptionType + "]" + network.rssi + "/";
  }

  WiFi.scanDelete();

  return wifiNetworksString;
}

void bt_begin() {
  Serial.println("Entering bluetooth setup");

  String wifiNetworksString = bt_listWifiAccessPoints();
  Serial.println(wifiNetworksString);

  BLEDevice::init(BT_DEVICE_NAME);
  BLEServer* pServer = BLEDevice::createServer();

  pServer->setCallbacks(new ServerCallbacks());
  BLEService* pService = pServer->createService(BT_SERVICE_UUID);

  BLEDescriptor btSSIDDescriptor(BT_SSID_DESCRIPTOR_UUID);
  BLEDescriptor btWifiPassDescriptor(BT_WIFIPASS_DESCRIPTOR_UUID);
  BLEDescriptor btWifiListDescriptor(BT_WIFI_LIST_DESCRIPTOR_UUID);
  BLEDescriptor btConfigDoneDescriptor(BT_CONFIG_DONE_DESCRIPTOR_UUID);

  btSSIDDescriptor.setValue(BT_SSID_CHARACTERISTIC_NAME);
  btWifiPassDescriptor.setValue(BT_WIFIPASS_CHARACTERISTIC_NAME);
  btWifiListDescriptor.setValue(BT_WIFI_LIST_CHARACTERISTIC_NAME);
  btConfigDoneDescriptor.setValue(BT_CONFIG_DONE_CHARACTERISTIC_NAME);

  BLECharacteristic btSSIDCharacteristic(
      BT_SSID_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  BLECharacteristic btWifiPassCharacteristic(
      BT_WIFIPASS_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  BLECharacteristic btWifiListCharacteristic(BT_WIFI_LIST_CHARACTERISTIC_UUID,
                                             BLECharacteristic::PROPERTY_READ);
  BLECharacteristic btConfigDoneCharacteristic(
      BT_CONFIG_DONE_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pService->addCharacteristic(&btSSIDCharacteristic);
  btSSIDCharacteristic.addDescriptor(&btSSIDDescriptor);
  btSSIDCharacteristic.setValue("");

  pService->addCharacteristic(&btWifiPassCharacteristic);
  btWifiPassCharacteristic.addDescriptor(&btWifiPassDescriptor);
  btWifiPassCharacteristic.setValue("");

  pService->addCharacteristic(&btWifiListCharacteristic);
  btWifiListCharacteristic.addDescriptor(&btWifiListDescriptor);
  btWifiListCharacteristic.setValue(wifiNetworksString.c_str());

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
  btWifiListCharacteristic.setCallbacks(
      new CharacCallbacks(BT_WIFI_LIST_CHARACTERISTIC_NAME));
  btConfigDoneCharacteristic.setCallbacks(
      new CharacCallbacks(BT_CONFIG_DONE_CHARACTERISTIC_NAME));

  Serial.println("Characteristic defined! Now you can read it in your phone!");

  isInBtSetup = true;

  for (;;) {
    String newWifiNetworksString = bt_listWifiAccessPoints();
    btWifiListCharacteristic.setValue(newWifiNetworksString.c_str());
    delay(BT_WIFI_SCAN_REFRESH_PERIOD);
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
