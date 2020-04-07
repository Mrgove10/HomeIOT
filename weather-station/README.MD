# Weather-Station

[THIS IS A REMAKE OF AN OLD PROJECT](https://github.com/Mrgove10/Weather-Station)

Weather station whit ESP8266 and nodeJS server

## Table of contents

- [Weather-Station](#weather-station)
  - [Table of contents](#table-of-contents)
  - [General informations](#general-informations)
  - [Hardware parts](#hardware-parts)
  - [Helpfull links](#helpfull-links)

## General informations

The goal of this project is to make a simple indoor weather station and send data at regular intervals to a server to store that date.

We are storing :

- Temperature
- Pressure
- Humidity

The server is waiting for requestes from the ESP3622 in JSON form to store them.

## Hardware parts

|      Part name      |                                  Description                                  | Price |                              Link                              |
| :-----------------: | :---------------------------------------------------------------------------: | :---: | :------------------------------------------------------------: |
| ESP8622 Node MCU v3 | Core of the wheater station, can connect to the internet and run arduino code | ~ 3$  | [AliExpress](https://www.aliexpress.com/item/32266751149.html) |
|       BME280        |                   Temperature, Pressure and humidity sensor                   | ~ 3$  |   [AliExpress](https://aliexpress.com/item/32830727102.html)   |
|      2004 LCD       |                               20*4 Caracter LCD                               | ~ 5$  | [AliExpress](https://www.aliexpress.com/item/32266751149.html) |
|  LCD I2C converter  |                     Allow the LCD to receive I2C commands                     | ~ 2$  | [AliExpress](https://fr.aliexpress.com/item/32890839189.html)  |

total cost : **~ 15$**

## Helpfull links

https://randomnerdtutorials.com/esp32-esp8266-i2c-lcd-arduino-ide/  
https://omerk.github.io/lcdchargen/
http://embedded-lab.com/blog/making-simple-weather-web-server-using-esp8266-bme280/