# HA Wordclock ESPHome

## General

This project is just for documentation purposes and our attempt (together with a friend) to build an Wordclock, which is thightly bound to homeassistant using esphome.
It uses the time and fastled plattform of esphome to controll the clock.

### Features

- Get time from HA instance
- Controll color and brightness from ha
- Controll status leds from ha with service calls
- uses esphome for OTA/API
- supports minutes LEDS

![Image of final wordclock](images/Front.png)

## Shopping list

- Mirror 30x30 (IKEA LOTS - 7.99EUR/4Pack)
- Woodframe
- Wood for Back and inlay
- ESP32
- 125x WS2812B LEDS
- Glue, wires and solder

## Maschines needed: (All files are attached in ZIP Package)

- Laser engraver

## Installation

- Mount the LED Stripes to the backplate and solder them based on following layout:

  ```plaintext
  ESXISTXFÜNF    1.......11
  ZEHNZWANZIG    22......12
  DREIVIERTEL    23......33
  XXNACHVORXX    44......34
  HALBXZWÖLFX    45......55
  ZWEINSIEBEN    ....
  XDREIXXFÜNF    ....
  ELFNEUNVIER    ....
  XACHTZEHNRS    ....
  XSECHSXXUHR    109.......101
  ICONS          110.......120
  MINUTES        124.......121
  ```

![Image of final wordclock](images/Back_leds.png)

- Connect Ground and 5V and Data Pin of WS2812 to GPIO5 on the ESP32
- Using esphome GUI you can flash wordclock.yaml to your ESP32

## Home Assistant integration

Add the esphome to your Home Assistant instance.
You should now see the Dummy Light "Wordclock", which allows you to controll the brightness and color of the Wordclock.
Additionaly there is a service esphome.wordclock_setled, which allow you to set color to individual LEDS (e.g. the status icons)
This Service takes: number (PIN Number of LED, see above), red (0-255), green (0-255), blue (0-255)

```yaml
service: esphome.wordclock_setled
data:
  number: 115
  green: 0
  red: 255
  blue: 0
```
