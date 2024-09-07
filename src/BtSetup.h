#pragma once

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <SD.h>

#include "constants.h"
#include "global.h"

void bt_begin();

String bt_listWifiAccessPoints();

bool bt_saveCredToSD();

bool bt_testWifi(String ssid, String pass);
