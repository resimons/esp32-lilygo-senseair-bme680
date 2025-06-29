![Build](https://github.com/KlausMu/esp32-co2monitor/actions/workflows/build-platformio.yml/badge.svg)

## Pin layout LilyGO TTGO T3 LoRa32 433MHz V1.6.1 ESP32

![image](https://github.com/umbm/TTGO-LoRa32-V2.1-T3_V1.6/blob/master/T3_1.6.jpg)

## Wiring

Component | Wire   | ESP
----------- |--------| --------------
BME680 VCC | RED    | 3V3
BME680 GND | BLACK  | GND
BME680 SCL | YELLOW | GPIO22
BME680 SDA |  GREEN | GPIO21
BME680 SDO | BLACK | GND
SENSEAIR UART RxD | BLUE | TxD GPIO1 
SENSEAIR UART TxD | GREEN | RxD GPIO3
SENSEAIR VCC | RED | 5V
SENSEAIR GND | BLACK | GND

Note that the TxD/Rxd pins are also used for the serial device for uploading firmware. 
So, for uploading firmware the BMD680 VCC should be disconnected.

## I2C

Note that SDO of BME680 is connected to the ground. This forces the sensors address to 0x76 in order to void interference with the I2C of the display.

## Documentation

[LilyGo](https://lilygo.cc/products/lora3)
[Board pins](https://github.com/Xinyuan-LilyGo/TTGO-LoRa-Series)
