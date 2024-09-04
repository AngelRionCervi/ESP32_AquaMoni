#pragma once

#define PH_ACID_VOLTAGE 2030
#define PH_NEUTRAL_VOLTAGE 1485
#define DEBOUNCE_PERIOD 10
#define NTP_SERVER_ADDRESS "europe.pool.ntp.org"
#define FILE_LAST "/last.jso"
#define FILE_CONFIG "/config.jso"
#define MAX_WIFI_TRIES 20

#define BUTTON_0_PIN 32
#define BUTTON_1_PIN 27
#define BUTTON_2_PIN 13
#define BUTTON_3_PIN 12
#define BUTTON_4_PIN 14

#define LED_0_PIN 21
#define LED_1_PIN 3
#define LED_2_PIN 1
#define LED_3_PIN 22
#define LED_4_PIN 2

#define BUTTON_SCHEDULE_PIN 33
#define SCHEDULE_LED_GREEN_PIN 25
#define SCHEDULE_LED_RED_PIN 26

#define SPI_CLK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define SPI_CS 5

#define PH_SENSOR_PIN 35
#define TEMP_SENSOR_PIN 15

#define ACTIVITY_LED_GREEN_PIN 0
#define ACTIVITY_LED_RED_PIN 4

#define BT_DEVICE_NAME "Aqua Pal"
#define BT_SERVICE_UUID "0880619c-4a5f-4420-a3fa-496713d52298"

#define BT_SSID_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BT_WIFIPASS_CHARACTERISTIC_UUID "30855ddd-1312-43be-a6e1-1b8d188c43d2"
#define BT_WIFI_LIST_CHARACTERISTIC_UUID "9b148ccd-66d1-49e2-9642-074d9ab49897"
#define BT_CONFIG_DONE_CHARACTERISTIC_UUID "f4187856-ff8e-402a-b609-1486b775c94f"

#define BT_SSID_DESCRIPTOR_UUID "3f829220-a7f7-4b9c-bb0b-9dca188ad6d3"
#define BT_WIFIPASS_DESCRIPTOR_UUID "e6fca03a-0c70-4eb7-aa36-9d3e830f93a1"
#define BT_WIFI_LIST_DESCRIPTOR_UUID "d4b5e976-2981-4f8d-9327-290a79c47667"
#define BT_CONFIG_DONE_DESCRIPTOR_UUID "134669c1-2043-45ca-80cb-63d5466f399f"

#define BT_SSID_CHARACTERISTIC_NAME "ssid"
#define BT_WIFIPASS_CHARACTERISTIC_NAME "wifi_pass"
#define BT_WIFI_LIST_CHARACTERISTIC_NAME "wifi_list"
#define BT_CONFIG_DONE_CHARACTERISTIC_NAME "config_done"

#define BT_WIFI_SCAN_REFRESH_PERIOD 10000
