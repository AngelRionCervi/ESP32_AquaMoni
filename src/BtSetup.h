#pragma once

#include <SD.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "constants.h"
#include "global.h"

void bt_begin();

void bt_readSerial();

bool bt_saveCredToSD();
