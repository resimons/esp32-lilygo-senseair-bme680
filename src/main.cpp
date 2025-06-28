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
#include <Adafruit_BME680.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "log.h"
#include "senseair_s8.h"

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
Adafruit_BME680 bme; // I2C

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

  if (!bme.begin(0x76)) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  Serial.println("Hier3");
  // co2_setup();

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

  // sendAlive(ssid);
}

void sendMessage(String outgoing);
void displayAndSendBmeValues();

void loop() {
  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  Serial.print(F("Reading started at "));
  Serial.print(millis());
  Serial.print(F(" and will finish at "));
  Serial.println(endTime);

  Serial.println(F("You can do other work during BME680 measurement."));
  delay(50); // This represents parallel work.
  // There's no need to delay() until millis() >= endTime: bme.endReading()
  // takes care of that. It's okay for parallel work to take longer than
  // BME680's measurement time.

  // Obtain measurement results from BME680. Note that this operation isn't
  // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

  displayAndSendBmeValues();

  delay(2000);
}

void displayAndSendBmeValues() {

    String temp = String(bme.readTemperature());
    String pressure = String(bme.readPressure() / 100);
    String humidity = String(bme.readHumidity());
    String gasResistance = String(bme.gas_resistance / 1000);
    String altitude = String(bme.readAltitude(SEALEVELPRESSURE_HPA));

    display.clearDisplay();
    display.setCursor(7,10);
    display.print("Temp: " + temp + " C");
    display.setCursor(7,22);
    display.print("Press:" + pressure + " hPa");
    display.setCursor(7,34);
    display.print("Hum:  " + humidity + " %");
    display.setCursor(7,46);
    display.print("Gas:  " + gasResistance + " m");
    display.setCursor(7,58);
    display.print("SSID: " + String(ssid) + " m");
    display.display();

    String payload = "";
    payload += "{\"temperature\":";
    payload += temp;
    payload += ",\"pressure\":";
    payload += pressure;
    payload += ",\"humidity\":";
    payload += humidity;
    payload += ",\"gas_resistance\":";
    payload += gasResistance;
    payload += ",\"altitude\":";
    payload += altitude;
    payload += ",\"sensor\":";
    payload += "\"bme680\"";
    payload += ",\"device\":";
    payload += "\"";
    payload += ssid;
    payload += "\"";
    payload += "}";

    sendMessage(payload);
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  Serial.println(outgoing);
}
