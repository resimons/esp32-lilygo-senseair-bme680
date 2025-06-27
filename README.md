![Build](https://github.com/KlausMu/esp32-co2monitor/actions/workflows/build-platformio.yml/badge.svg)

## Connections

## Pin layout ESP32

![image](https://github.com/umbm/TTGO-LoRa32-V2.1-T3_V1.6/blob/master/T3_1.6.jpg)


Component | Wire   | ESP
----------- |--------| --------------
BME280 VCC | RED    | 3V3
BME280 GND | BLACK  | GND
BME280 SCL | YELLOW | IO32
BME280 SDA |  GREEN | IO33
SENSEAIR UART RxD | BLUE | TxD IO5 
SENSEAIR UART TxD | GREEN | RxD IO5
SENSEAIR VCC | RED | 5V
SENSEAIR GND | BLACK | GND
