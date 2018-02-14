#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <DHTesp.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <WiFiManager.h>

// define MQTT broker details in this include file. See secrets.h.sample
#include "secrets.h"

// name used in WiFiManager and OTA together with the last three bytes of the MAC address
#define NAME "envmon"

DHTesp dht;
PubSubClient mqtt;
Ticker measureTicker;
WiFiClient client;
WiFiManager wifiManager;

char myname[32];
int measure_now = 0;


static void set_myname() {
  byte mac[6];

  WiFi.macAddress(mac);
  snprintf(myname, sizeof(myname), "%s-%02x-%02x-%02x", NAME, mac[3], mac[4], mac[5]);
  ArduinoOTA.setHostname(myname);
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  set_myname();
  Serial.print("My name: ");
  Serial.println(myname);
  wifiManager.autoConnect(myname);
  ArduinoOTA.begin();
  
  Serial.println("Connecting to MQTT");
  mqtt.setClient(client);
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.connect(myname, MQTT_USER, MQTT_PASS);

  Serial.println("Starting sensor");
  dht.setup(D0, DHTesp::AM2302);
  Serial.print("Model: ");
  Serial.println(dht.getModel());
  measureTicker.attach(10.0, measure_trigger);
}


void measure() {
  char v[16], t[256];
  struct TempAndHumidity th;
  
  if (!measure_now)
    return;

  measure_now = 0;

  Serial.println("Measuring");
  th = dht.getTempAndHumidity();
  if (dht.getStatus() != DHTesp::ERROR_NONE) {
    Serial.print("DHT error: ");
    Serial.println(dht.getStatusString());
    return;
  }
  dtostrf(th.temperature, 5, 1, v);
  Serial.print("Temperature: ");
  Serial.println(v);
  snprintf(t, sizeof(t), "/%s/%s/temp", MQTT_TOPIC, myname);
  mqtt.publish(t, v);

  dtostrf(th.humidity, 5, 1, v);
  Serial.print("Humidity: ");
  Serial.println(v);
  snprintf(t, sizeof(t), "/%s/%s/hum", MQTT_TOPIC, myname);
  mqtt.publish(t, v);
}


void measure_trigger() {
  measure_now++;
}


void loop() {
  ArduinoOTA.handle();
  measure();
  delay(10);
}
