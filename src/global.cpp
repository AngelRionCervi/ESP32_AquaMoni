#include "global.h"

#include "constants.h"

bool sensorError = false;
bool areSchedulesDisabled = false;
std::unordered_map<std::string, Device> devices;

WiFiClient wifiClient;
WebServer server(80);

int scheduleUpdatePeriode = 1000;
int scheduleUpdateLastMillis = 0;

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
