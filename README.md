# Connect a BME680 to a LilyGO TTGO T3 LoRa32 433MHz V1.6.1 ESP32

The ESP will read the data from the sensors and puts it over the air.
The data is received by another TTGO LoRa32 using [OpenMQTTGateway](https://docs.openmqttgateway.com/)

## Pin layout LilyGO TTGO T3 LoRa32 433MHz V1.6.1 ESP32

![image](https://github.com/umbm/TTGO-LoRa32-V2.1-T3_V1.6/blob/master/T3_1.6.jpg)

![image](https://www.tinytronics.nl/image/cache/catalog/products/product-003658/bme680-module-gas-temperature-pressure-and-humidity-sensor-with-level-converter-1200x1200.jpg)

![image](https://www.tinytronics.nl/image/cache/data/product-3207/senseair-s8-lp-co2-sensor-004-0-0053-front-1500x1500.jpg)

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

## Where to buy

[LoRa32](https://www.tinytronics.nl/nl/development-boards/microcontroller-boards/met-lora/lilygo-ttgo-t3-lora32-433mhz-v1.6.1-esp32)
[BME680](https://www.tinytronics.nl/nl/sensoren/lucht/druk/bme680-sensor-module-met-level-converter-luchtdruk-luchtkwaliteit-luchtvochtigheid-temperatuur)
[SenseAir S8](https://www.tinytronics.nl/nl/sensoren/lucht/gas/senseair-s8-lp-co2-sensor-004-0-0053)

## Documentation

[LilyGo](https://lilygo.cc/products/lora3)
[Board pins](https://github.com/Xinyuan-LilyGo/TTGO-LoRa-Series)
[BME680](https://www.tinytronics.nl/product_files/003658_bme680-module-gas-temperature-pressure-and-humidity-sensor-with-level-converter-datasheet.pdf)
[SenseAir S8](https://www.tinytronics.nl/product_files/003207_Senseair_S8_LP_datasheet.pdf)
