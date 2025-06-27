#include <Arduino.h>
#include <LoRa.h>

#include "senseair_s8.h"
#include "bme680.h"

void sendMessage(String message);

void sendAlive(String ssid) {

  // maximum message length 128 Byte
  String payload = "";
  char buffer [50];

  payload = "";
  payload += "{\"device\":";
  payload += "\"";
  payload += ssid;
  payload += "\"";
  payload += "}\n";
  sendMessage(payload.c_str());

  Serial.printf("I'm alive is sent!");
}

void sendMeasurements(String ssid) {
  // maximum message length 128 Byte
  String payload = "";
  char buffer [50];

  // co2. Don't send 0 values. Minimum = 400
  if (co2_value > 0) {
    payload = "";
    payload += "{\"co2\":";
    payload += String(co2_value);
    payload += ",\"co2status\":";
    payload += String(co2_status);
    payload += ",\"device\":";
    payload += "\"";
    payload += ssid;
    payload += "\"";
    payload += "}\n";
    sendMessage(payload.c_str());
  }

  // bme680
  payload = "";
  payload += "{\"temperature\":";
  payload += String(temperature);
  payload += ",\"pressure\":";
  payload += String(pressure);
  payload += ",\"humidity\":";
  payload += String(humidity);
  payload += ",\"gas_resistance\":";
  payload += String(gasResistance);
  payload += ",\"device\":";
  payload += "\"";
  payload += ssid;
  payload += "\"";
  payload += "}\n";
  sendMessage(payload.c_str());
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                    // finish packet and send it
  Serial.println("Message sent!");
}
