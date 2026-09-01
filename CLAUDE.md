# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Firmware for a LilyGO TTGO T3 LoRa32 433MHz V1.6.1 ESP32 board that reads environmental data from two sensors and transmits it via LoRa radio:
- **BME680** (I2C at 0x76): temperature, pressure, humidity, gas resistance
- **SenseAir S8** (UART): CO2 concentration and sensor status

Data is received by another TTGO LoRa32 running [OpenMQTTGateway](https://docs.openmqttgateway.com/).

## Build & Flash Commands

Uses PlatformIO. Target environment is `ttgo-lora32-v2`.

```bash
# Build
pio run -e ttgo-lora32-v2

# Upload firmware (disconnect SenseAir S8 VCC first — RX/TX pins are shared with upload interface)
pio run -e ttgo-lora32-v2 --target upload

# Monitor serial output at 115200 baud
pio device monitor -b 115200

# Build + upload + monitor
pio run -e ttgo-lora32-v2 --target upload && pio device monitor -b 115200
```

## Architecture

### Main loop (`src/main.cpp`)
The `loop()` alternates every 30 seconds:
1. Request CO2 + status from SenseAir via `co2_requestValueAndStatus()`
2. Read BME680 and send payload via LoRa → `displayAndSendBmeValues()`
3. 30s delay
4. Send CO2 payload via LoRa → `displayAndSendCO2Value()`
5. If the heartbeat interval has elapsed, send a heartbeat payload via LoRa → `publish_heartbeat()`
6. 30s delay

Each payload is a JSON string sent as a raw LoRa packet. The device identifies itself using `ssid` (derived from `ESP.getEfuseMac()`) and MAC address.

### Heartbeat (`src/main.cpp`)
`publish_heartbeat()` sends a `"type":"heartbeat"` payload once at startup and then periodically, gated by a non-blocking `millis()` comparison against `lastHeartbeatMillis` (so it doesn't disturb the blocking sensor delays). The interval is configurable via `HEARTBEAT_INTERVAL_MINUTES` in the `/* BEGIN CONFIGURATION */` block at the top of `main.cpp` (default 5 minutes).

### SenseAir S8 driver (`src/senseair_s8.cpp/.h`)
Custom Modbus/UART driver using `HardwareSerial(2)` on GPIO 1 (TXD2) / GPIO 3 (RXD2). Implements:
- `co2_requestValueAndStatus()` — combined register read (preferred, used in main loop)
- `co2_requestValue()` / `co2_requestStatus()` — individual register reads
- Background calibration workflow: `co2_clearBackgroundCalibrationAck()` → `co2_startBackgroundCalibration()` → `co2_checkBackgroundCalibrationAck()`
- `calibrationStatus` global tracks calibration state (0=idle, 1=starting, 2=awaiting ack, 3=ack received)

### Logging (`src/log.cpp/.h`)
`LogStreamClass Log` multiplexes output to Serial and/or TelnetStream, controlled by `#define useSerial` and `#define useTelnetStream` in `config.h`.

### Configuration (`src/config.h`)
Contains Wi-Fi credentials, MQTT topic names, timezone, and feature flags. **This file contains plaintext credentials** — do not commit changes that expose new secrets.

## Hardware Notes

- **Upload constraint**: SenseAir S8 shares GPIO 1/3 with the USB-serial upload interface. Disconnect SenseAir VCC before flashing.
- **BME680 I2C address**: SDO tied to GND → address 0x76 (avoids conflict with OLED at 0x3C).
- **LoRa settings**: 433 MHz, SF7, BW 125 kHz, CR 4/5, sync word 0x12, preamble 8.
- CO2 values below 400 ppm are suppressed (sensor minimum); values above 4000 ppm are treated as malfunction and set to 0.
