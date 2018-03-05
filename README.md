# blinkenlights
Show random signal aspects using ESP8266 modules

## Hardware

We use Wemos D1 mini ESP8266 modules (or clones), with a number of [SI2302DS](https://www.vishay.com/docs/70628/70628.pdf) (PDF) N-channel MOSFETs as low-side switches.

We use XLR five-position connectors: female in the enclosure for the ESP8266, male on the consumers. The pin assignments are:

| XLR Pin | Cable  | Signal | Arduino |
| ------- | ------ | ------ | ------- |
| 1       | blue   | Out 1  | Pin D7  |
| 2       | yellow | Out 2  | Pin D6  |
| 3       | grey   | Out 3  | Pin D2  |
| 4       | black  | Out 4  | Pin D1  |
| 5       | brown  | +12V=  | -       |

## PCB

[d1mini-sw](d1mini-sw) contains Eagle files and matching Gerbers for a board compatible with the Wemos D1 Mini. For all seven GPIOs that can be used freely, a BSS138 (or compatible) MOSFET is connected as a low-side switch; the switched outputs are brought out on nine pads with 100mil spacing.  Input voltage for the ESP as well as a Vdd net is brought our on six pads with 100mil spacing.

## Sample Code

The Arduino Sketch [cycle-four-outputs](cycle-four-outputs) is a simple test programs that cycles on each of the four outputs in sequence, then turns them off, then starts over.

It also shows how to use [WiFi Manager](https://github.com/tzapu/WiFiManager) and [Arduino OTA](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html).


### envmonitor

[envmonitor](envmonitor) shows how to read a DHT22 connected to pin D0 and post the data to MQTT. MQTT broker paramters have to be defined in [envmonitor/secrets.h](envmonitor/secrets.h), which is not checked in. See [envmonitor/secrets.h.example](envmonitor/secrets.h.example) for a sample file.
