#pragma once
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <map>
#include <unordered_map>

#include "ActivityLed.h"
#include "Device.h"
#include "ScheduleButton.h"

#ifndef MY_GLOBALS_H
#define MY_GLOBALS_H

// This is a declaration of your variable, which tells the linker this value
// is found elsewhere.  Anyone who wishes to use it must include global.h,
// either directly or indirectly.
extern bool sensorError;
extern bool areSchedulesDisabled;
extern bool isInBtSetup;
extern bool btDeviceConnected;
extern std::map<int, int> buttonMap;
extern std::map<int, int> ledMap;
extern std::map<String, float> monitoringLiveMap;
extern std::unordered_map<std::string, Device> devices;
extern WiFiClient wifiClient;
extern WebServer server;
extern WebSocketsClient webSocket;
extern String wifiSSID;
extern String wifiPass;
extern String serverPass;
extern int autoSchedulesOnAfter;
extern bool enableMonitoring;
extern bool phCalibrationState;
extern ScheduleButton scheduleButton;
extern ActivityLed activityLed;
extern int scheduleUpdatePeriode;
extern int scheduleUpdateLastMillis;
extern int devicesStatesUpdatePeriode;
extern int devicesStatesUpdateLastMillis;
extern int measurementsUpdatePeriode;
extern int measurementsUpdateLastMillis;
extern int scheduleOnPeriode;
extern int scheduleOnLastMillis;
extern int sendLastMeasurementsUpdatePeriode;
extern int sendLastMeasurementsMillis;
extern int updateLiveMeasurementsPeriode;
extern int liveMeasurementsLastMillis;
extern int sendLiveMeasurementsUpdatePeriode;
extern int sendLiveMeasurementsLastMillis;
extern int phMvCalibrationPeriode;
extern int phMvCalibrationLastMillis;
extern int phCalibrationMv;
extern int phCalibration4Mv;
extern int phCalibration7Mv;
extern String boxId;

#endif