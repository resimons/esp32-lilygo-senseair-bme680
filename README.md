![Build](https://github.com/KlausMu/esp32-co2monitor/actions/workflows/build-platformio.yml/badge.svg)

## Connections

## Pin layout ESP32

![image](https://github.com/umbm/TTGO-LoRa32-V2.1-T3_V1.6/blob/master/T3_1.6.jpg)


Component | Wire   | ESP
----------- |--------| --------------
BME680 VCC | RED    | 3V3
BME680 GND | BLACK  | GND
BME680 SCL | YELLOW | GPIO22
BME680 SDA |  GREEN | GPIO21
SENSEAIR UART RxD | BLUE | TxD GPIO1 
SENSEAIR UART TxD | GREEN | RxD GPIO3
SENSEAIR VCC | RED | 5V
SENSEAIR GND | BLACK | GND
