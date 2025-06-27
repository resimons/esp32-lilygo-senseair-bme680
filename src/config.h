#include <driver/gpio.h>
#include <esp32-hal-gpio.h>

// --- wifi ---------------------------------------------------------------------------------------------------------------------------------
const char* const wifi_ssid              = "ROELS (2G)";
const char* const wifi_password          = "6252915687382571";

// --- OTA Update ---------------------------------------------------------------------------------------------------------------------------
#define useOTAUpdate
// #define useOTA_RTOS     // not recommended because of additional 10K of heap space needed

#if !defined(ESP32) && defined(useOTA_RTOS)
static_assert(false, "\"#define useOTA_RTOS\" is only possible with ESP32");
#endif
#if defined(useOTA_RTOS) && !defined(useOTAUpdate)
static_assert(false, "You cannot use \"#define useOTA_RTOS\" without \"#define useOTAUpdate\"");
#endif

#define useSerial
#define useTelnetStream

// --- mqtt ---------------------------------------------------------------------------------------------------------------------------------
const char* const mqtt_server           = "cleopatra";
const int mqtt_server_port              = 8883;

const char* const mqttTopicSenseAir     = "topic/senseair8/co2";
const char* const mqttTopicBme680       = "topic/bme680/sensor";
const char* const mqqtTopicAlive         = "topic/iamalive";


// --- language and timezone ----------------------------------------------------------------------------------------------------------------
#include "lang/en.h"

// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
#define myTimezone                 "Europe/Berlin"

// --- COVID-19 incidences ------------------------------------------------------------------------------------------------------------------
// Only activate if you are in Germany. Set your region in file "incidenceMap.cpp"
#define useIncidenceFromRKI

// ------------------------------------------------------------------------------------------------------------------------------------------

/*
ADC1
32 33 34 35 36 (37) (38) 39

ADC2 (WIFI)
00 02 04 12 13 14 15 25 26 27
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/adc.html
The ESP32 integrates two 12-bit SAR (Successive Approximation Register) ADCs, supporting a total of 18 measurement channels (analog enabled pins).
The ADC driver API supports ADC1 (8 channels, attached to GPIOs 32 - 39), and ADC2 (10 channels, attached to GPIOs 0, 2, 4, 12 - 15 and 25 - 27). However, the usage of ADC2 has some restrictions for the application:
ADC2 is used by the Wi-Fi driver. Therefore the application can only use ADC2 when the Wi-Fi driver has not started.
Some of the ADC2 pins are used as strapping pins (GPIO 0, 2, 15) thus cannot be used freely. Such is the case in the following official Development Kits:
ESP32 DevKitC: GPIO 0 cannot be used due to external auto program circuits.
ESP-WROVER-KIT: GPIO 0, 2, 4 and 15 cannot be used due to external connections for different purposes.
*/
