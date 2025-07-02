
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

/* BEGIN CONFIGURATION */
#define DEBUG_BAUDRATE 115200

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

#define BME280_ADDRESS_ALTERNATE 0x76

unsigned long previousMillisProbe = 0;
unsigned long intervalProbe = 4000;      // this is the internal update interval of the CO2 sensor

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME680 bme; // I2C

char ssid[23];

void setup() {

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

  if (!bme.begin(BME280_ADDRESS_ALTERNATE)) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  display.setCursor(5,10);
  display.print("BME680 found");


  co2_setup();
  
  LoRa.setPins(LORA_CS, LORA_RESET, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ)) {
    // Serial.println("Starting LoRa failed!");
    while (1);
  }

  display.setCursor(5,25);
  display.print("LORA OK");
  display.setCursor(5,40);
  display.print("Sampling started");
  display.setCursor(5,55);
  display.print(String(ssid).substring(10));
  
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
void displayAndSendCO2Value();

void loop() {

  co2_requestValueAndStatus();

  if (calibrationStatus == 2) {
    co2_checkBackgroundCalibrationAck();
  }

  display.clearDisplay();

  displayAndSendBmeValues();

  delay(10000);

  displayAndSendCO2Value();

  delay(10000);
}

void displayAndSendCO2Value() {
    display.setCursor(7,55);
    display.print("CO2:  " + String(co2_value) + " ppm");
    display.display();

  // co2. Don't send 0 values. Minimum = 400
  if (co2_value > 0) {
    String payload = "";
    payload += "{\"co2\":";
    payload += String(co2_value);
    payload += ",\"status\":";
    payload += String(co2_status);
    payload += ",\"sensor\":";
    payload += "\"senseairs8\"";
    payload += ",\"device\":";
    payload += "\"";
    payload += ssid;
    payload += "\"";
    payload += "}";

    sendMessage(payload);
  }
}

void displayAndSendBmeValues() {

    if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

    String temperature = String(bme.temperature);
    String pressure = String(bme.pressure / 100);
    String humidity = String(bme.humidity);
    String gasResistance = String(bme.gas_resistance / 1000);
    String altitude = String(bme.readAltitude(SEALEVELPRESSURE_HPA));

    display.setCursor(7,7);
    display.print("Temp: " + temperature + " C");
    display.setCursor(7,19);
    display.print("Press:" + pressure + " hPa");
    display.setCursor(7,31);
    display.print("Hum:  " + humidity + " %");
    display.setCursor(7,43);
    display.print("Gas:  " + gasResistance + " kOhms");
    display.display();

    String payload = "";
    payload += "{\"temperature\":";
    payload += temperature;
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
}
