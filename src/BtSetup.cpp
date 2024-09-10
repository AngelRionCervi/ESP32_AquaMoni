#include "BtSetup.h"

String tempSSID = "";
String tempWifiPass = "";

BLECharacteristic btSSIDCharacteristic(BT_SSID_CHARACTERISTIC_UUID,
                                       BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE);

BLECharacteristic btWifiPassCharacteristic(
    BT_WIFIPASS_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

BLECharacteristic btWifiListCharacteristic(BT_WIFI_LIST_CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ);

BLECharacteristic btConfigDoneCharacteristic(
    BT_CONFIG_DONE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

BLECharacteristic btWifiTestedCharacteristic(
    BT_WIFI_TESTED_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE |
        BLECharacteristic::PROPERTY_WRITE);

BLECharacteristic btRestartCharacteristic(
    BT_RESTART_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

BLEDescriptor btSSIDDescriptor(BT_SSID_DESCRIPTOR_UUID);
BLEDescriptor btWifiPassDescriptor(BT_WIFIPASS_DESCRIPTOR_UUID);
BLEDescriptor btWifiListDescriptor(BT_WIFI_LIST_DESCRIPTOR_UUID);
BLEDescriptor btConfigDoneDescriptor(BT_CONFIG_DONE_DESCRIPTOR_UUID);
BLEDescriptor btRestartDescriptor(BT_RESTART_DESCRIPTOR_UUID);

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

    if (!value.length()) {
      return;
    }

    if (name == BT_SSID_CHARACTERISTIC_NAME) {
      tempSSID = value;
      btWifiTestedCharacteristic.setValue("false");
    } else if (name == BT_WIFIPASS_CHARACTERISTIC_NAME) {
      tempWifiPass = value;
      btWifiTestedCharacteristic.setValue("false");
    } else if (name == BT_CONFIG_DONE_CHARACTERISTIC_NAME) {
      Serial.println("Config done: " + value);
      Serial.println("SSID: " + tempSSID);
      Serial.println("Password: " + tempWifiPass);
      if (value == "true" && tempSSID.length() > 0 &&
          tempWifiPass.length() > 0) {
        bool testWifiResult = bt_testWifi(tempSSID, tempWifiPass);
        if (testWifiResult) {
          Serial.println("WiFi credentials are correct");
          btWifiTestedCharacteristic.setValue("true");
          btWifiTestedCharacteristic.notify();
          bool isSaved = bt_saveCredToSD();
          if (isSaved) {
            Serial.println("WiFi credentials saved to SD card");
          } else {
            Serial.println("Failed to save WiFi credentials to SD card");
          }
        } else {
          btWifiTestedCharacteristic.setValue("error");
          btWifiTestedCharacteristic.notify();
        }
      }
    } else if (name == BT_RESTART_CHARACTERISTIC_NAME) {
      if (value == "true") {
        ESP.restart();
      }
    }
  }

  void onRead(BLECharacteristic* pCharacteristic) {
    Serial.println("Characteristic read: " + name);
  }
};

bool bt_testWifi(String ssid, String pass) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid.c_str(), pass.c_str());

  int wifiTries = 0;
  Serial.print("WiFi Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    activityLed.update();
    delay(500);
    wifiTries++;
    if (wifiTries > MAX_WIFI_TRIES) {
      Serial.println("Failed to connect to WiFi");
      activityLed.setState("error");
      activityLed.update();
      return false;
    }
  }
  Serial.println();
  return true;
}

String bt_listWifiAccessPoints() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Wifi scan started");

  struct WifiNetwork {
    String ssid;
    String encryptionType;
    int channel;
    int rssi;
  };

  std::map<int, WifiNetwork> wifiNetworksMap;

  int totalNetworks = WiFi.scanNetworks();

  if (totalNetworks == 0) {
    Serial.println("No networks found");
  } else {
    for (int i = 0; i < totalNetworks; ++i) {
      WifiNetwork wifiNetwork;
      String encryption;
      wifiNetwork.ssid = WiFi.SSID(i);
      wifiNetwork.channel = WiFi.channel(i);
      wifiNetwork.rssi = WiFi.RSSI(i);

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

  String wifiNetworksString;
  JsonDocument wifiNetworksJson;
  JsonArray wifiNetworksJsonArray = wifiNetworksJson.to<JsonArray>();

  int minRSSI = -80;
  int maxLength = 5;

  for (auto const& [_, network] : wifiNetworksMap) {
    if (network.rssi < minRSSI && wifiNetworksJsonArray.size() >= maxLength) {
      continue;
    }
    for (int i = 0; i < wifiNetworksJsonArray.size(); i++) {
      if (wifiNetworksJsonArray[i]["ssid"] == network.ssid) {
        continue;
      }
    }
    JsonDocument wifiNetworkJson;
    wifiNetworkJson["ssid"] = network.ssid;
    wifiNetworkJson["encryptionType"] = network.encryptionType;
    wifiNetworkJson["rssi"] = network.rssi;
    wifiNetworkJson["channel"] = network.channel;
    wifiNetworksJsonArray.add(wifiNetworkJson);
  }

  serializeJson(wifiNetworksJsonArray, wifiNetworksString);

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
  BLEService* pService = pServer->createService(BLEUUID(BT_SERVICE_UUID), 32);

  btSSIDDescriptor.setValue(BT_SSID_CHARACTERISTIC_NAME);
  btWifiPassDescriptor.setValue(BT_WIFIPASS_CHARACTERISTIC_NAME);
  btWifiListDescriptor.setValue(BT_WIFI_LIST_CHARACTERISTIC_NAME);
  btConfigDoneDescriptor.setValue(BT_CONFIG_DONE_CHARACTERISTIC_NAME);
  btRestartDescriptor.setValue(BT_RESTART_CHARACTERISTIC_NAME);

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

  pService->addCharacteristic(&btWifiTestedCharacteristic);
  btWifiTestedCharacteristic.addDescriptor(new BLE2902());
  btWifiTestedCharacteristic.setValue("false");

  pService->addCharacteristic(&btRestartCharacteristic);
  btRestartCharacteristic.addDescriptor(&btRestartDescriptor);
  btRestartCharacteristic.setValue("false");

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
  btWifiTestedCharacteristic.setCallbacks(
      new CharacCallbacks(BT_WIFI_TESTED_CHARACTERISTIC_NAME));
  btRestartCharacteristic.setCallbacks(
      new CharacCallbacks(BT_RESTART_CHARACTERISTIC_NAME));

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
