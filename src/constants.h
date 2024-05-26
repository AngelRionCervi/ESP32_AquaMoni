#pragma once

#define PH_ACID_VOLTAGE 2030
#define PH_NEUTRAL_VOLTAGE 1485
#define DEBOUNCE_PERIOD 10
#define NTP_SERVER_ADDRESS "europe.pool.ntp.org"
#define FILE_LAST "/last.jso"
#define FILE_CONFIG "/config.jso"

#define RELAY_FILTER_NAME "filter"
#define RELAY_FILTER_IP "192.168.1.16"

#define RELAY_SKIMMER_NAME "skimmer"
#define RELAY_SKIMMER_IP "192.168.1.13"

#define RELAY_CO2_NAME "co2"
#define RELAY_CO2_IP "192.168.1.15"

#define RELAY_LIGHT_NAME "light"
#define RELAY_LIGHT_IP "192.168.1.14"

#define BUTTON_0_PIN 14
#define BUTTON_1_PIN 12
#define BUTTON_2_PIN 13
#define BUTTON_3_PIN 27
#define BUTTON_4_PIN 32

#define LED_0_PIN 2
#define LED_1_PIN 22
#define LED_2_PIN 1
#define LED_3_PIN 3
#define LED_4_PIN 21

#define BUTTON_SCHEDULE_PIN 33
#define SCHEDULE_LED_GREEN_PIN 25
#define SCHEDULE_LED_RED_PIN 26

#define SPI_CLK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define SPI_CS 5

#define PH_SENSOR_PIN 36
#define TEMP_SENSOR_PIN 39
#define LED_GREEN_PIN 34
#define LED_RED_PIN 35