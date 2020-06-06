# HomeIOT

My home IOT devices, Composed of 2 weather stations.

- [HomeIOT](#homeiot)
  - [Inside Station](#inside-station)
    - [Components](#components)
    - [Wiring Diargram](#wiring-diargram)
    - [Code explaination](#code-explaination)
      - [Getting sensor data](#getting-sensor-data)
      - [Displaying data on the screen](#displaying-data-on-the-screen)
      - [Sending Data](#sending-data)
  - [Outside station](#outside-station)
    - [Components](#components-1)
    - [Code explaination](#code-explaination-1)
      - [Getting sensor data](#getting-sensor-data-1)
  - [Server side](#server-side)
    - [For the weather stations](#for-the-weather-stations)
    - [For the Openweathermap API](#for-the-openweathermap-api)
    - [For the database](#for-the-database)
    - [For the monitoring](#for-the-monitoring)
  - [Data visualisation](#data-visualisation)
  - [Helpfull links](#helpfull-links)


## Inside Station

### Components

The componentes in this build are the folowwing :

|           part            |               role               |
| :-----------------------: | :------------------------------: |
| ESP8266 (nodemcu variant) |         microcontroller          |
|          BME280           | temperature, humidity, pressure  |
|       0.96 Oled I2C       | display for temperature and time |

### Wiring Diargram

![](docs/WireDiag.png)

### Code explaination

[See project Code](inside_station/src/main.ino)

This project uses mqtt to send the data. I am utilising the test broker for mosquitto (Was too lazy to set up my own broker).

#### Getting sensor data


#### Displaying data on the screen


#### Sending Data

The data is send in a json string so it is easier to manage server side.
Example string send from the esp card
```json
{
  "t_in":"25.4",
  "p":"1200",
  "h_in":"45.3"
}
```

## Outside station

### Components

The componentes in this build are the folowwing :

|           part            |               role               |
| :-----------------------: | :------------------------------: |
| ESP8266 (nodemcu variant) |         microcontroller          |
|          ds18b20           | temperature  |


This project uses mqtt to send the data. I am utilising the test broker for mosquitto (Was too lazy to set up my own broker).7

### Code explaination

[See project Code](outside_station/src/main.ino)

#### Getting sensor data


## Server side

Now on node red ! You can find the node red script [here](node-red.json)

The code doesnt to any treament of the data, it only stores the values in the database

Basicly what happens :

### For the weather stations

1) Node red received the MQTT Values from each station
2) Wites those values to a file

### For the Openweathermap API

1) Every 2 minutes get the data from openWeatherMap
2) Write them to a file

### For the database

1) Every 1minute read from the 3 files (inside, outside & api)
2) Calclate the Humidex
3) Join those 3 fies into a single JSON
4) Insert the JSON into the database
  
### For the monitoring

1) When the database is calculated get the temperature
2) chek if it is over 28°C or under 15°C
3) Send request to IFTTT
4) Receive the according notification on your phone

## Data visualisation

I ♥ stats and graphs.
With that said the only logical choice is to use graphana
You can find my dashboard preset [here](grafana.json)

![](/docs/graph.png)

## Helpfull links

- <https://randomnerdtutorials.com/esp32-esp8266-i2c-lcd-arduino-ide/>
- <https://omerk.github.io/lcdchargen/>
- <http://embedded-lab.com/blog/making-simple-weather-web-server-using-esp8266-bme280/>
