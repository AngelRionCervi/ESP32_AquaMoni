#include "global.h"

#include "constants.h"

bool sensorError = false;
bool areSchedulesDisabled = false;
std::unordered_map<std::string, Device> devices;

WiFiClient wifiClient;
WebServer server(80);
WebSocketsClient webSocket;

int scheduleUpdatePeriode = 3000;
int devicesStatesUpdatePeriode = 3000; 
int measurementsUpdatePeriode = 10*60*1000; // every 10 mins
int sendLastMeasurementsUpdatePeriode = 10000;
int scheduleOnPeriode = 1000;
int scheduleUpdateLastMillis = 0;
int devicesStatesUpdateLastMillis = 0;
int measurementsUpdateLastMillis = 0;
int scheduleOnLastMillis = 0;
int sendLastMeasurementsMillis = 0;

String wifiSSID;
String wifiPass;
String serverPass;
String sessionId;
String boxId;
int autoSchedulesOnAfter;
bool enableMonitoring;

ScheduleButton scheduleButton(BUTTON_SCHEDULE_PIN, SCHEDULE_LED_GREEN_PIN, SCHEDULE_LED_RED_PIN);
ActivityLed activityLed(ACTIVITY_LED_GREEN_PIN, ACTIVITY_LED_RED_PIN);

std::map<int, int> buttonMap = {
    { 0, BUTTON_0_PIN },
    { 1, BUTTON_1_PIN },
    { 2, BUTTON_2_PIN },
    { 3, BUTTON_3_PIN },
    { 4, BUTTON_4_PIN },
};

std::map<int, int> ledMap = {
    { 0, LED_0_PIN },
    { 1, LED_1_PIN },
    { 2, LED_2_PIN },
    { 3, LED_3_PIN },
    { 4, LED_4_PIN },
};
