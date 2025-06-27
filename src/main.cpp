/*
  LoRa Duplex communication with Sync Word

  Sends a message every half second, and polls continually
  for new incoming messages. Sets the LoRa radio's Sync Word.

  The Sync Word is basically the radio's network ID. Radios with different
  Sync Words will not receive each other's transmissions. This is one way you
  can filter out radios you want to ignore, without making an addressing scheme.

  See the Semtech datasheet, http://www.semtech.com/images/datasheet/sx1276.pdf
  for more on Sync Word.

  created 28 April 2017
  by Tom Igoe
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "bme680.h"

#include "config.h"
#include "log.h"
#include "senseair_s8.h"
#include "storage.h"
#include "co2ValuesArray.h"
#include "timeHelper.h"
#include "messages.h"


#define LORA_CS 18
#define LORA_RESET 23
#define LORA_DIO0 26
#define LORA_FREQ 433E6

#define OLED_SDA 21
#define OLED_SCL 22 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// Checkout https://randomnerdtutorials.com/esp32-ssd1306-oled-display-arduino-ide/
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_ADDRESS 0x3C // See https://randomnerdtutorials.com/esp32-ssd1306-oled-display-arduino-ide/

#define SEALEVELPRESSURE_HPA (1013.25)

unsigned long previousMillisProbe = 0;
unsigned long intervalProbe = 4000;      // this is the internal update interval of the CO2 sensor
unsigned long previousMillisPublish = 0;
unsigned long intervalPublish = 10000;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char ssid[23];

void setup() {
  Serial.begin(115200);

   // Get deviceId
  snprintf(ssid, 23, "MCUDEVICE-%llX", ESP.getEfuseMac());

  Wire.begin(OLED_SDA, OLED_SCL);

  //initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  Serial.println("Hier");
  timeHelper_setup();

  Serial.println("Hier1");
  storage_init();
  Serial.println("Hier2");
  co2valuesArray_init();

  Serial.println("Hier3");
  co2_setup();

  Serial.println("Hier4");
  bme680Setup();

  Serial.println("Hier5");
  display.setCursor(10,10);
  display.print("BME680 found");

  LoRa.setPins(LORA_CS, LORA_RESET, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  display.setCursor(10,25);
  display.print("LORA OK");
  display.display();

  display.setCursor(10,40);
  display.print("Sampling started");
  display.display();

  delay(2500);

  LoRa.setPreambleLength(8);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);

  // weather monitoring
  Serial.println("-- Weather Station Scenario --");
  Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
  Serial.println("filter off");
  Serial.println("Hier6");

  sendAlive(ssid);
  Serial.println("Hier7");
}

void loop()
{

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisProbe >= intervalProbe)
  {
    previousMillisProbe = currentMillis;

    co2_requestValueAndStatus();
    co2values.addCO2value(co2_value);

    if (calibrationStatus == 2) {
      co2_checkBackgroundCalibrationAck();
    }

    bme680Read();
  }

  if (currentMillis - previousMillisPublish >= intervalPublish)
  {
    previousMillisPublish = currentMillis;

    timeHelper_update();

    // co2values.maintainValues();

    getNVSStatistics();

    sendMeasurements(ssid);
  }
}
