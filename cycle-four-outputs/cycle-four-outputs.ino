#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

static int outputs[] = { D7, D6, D2, D1 };

static void all_off() {
  for (int i=4; i--; ) {
    pinMode(outputs[i], OUTPUT);
    digitalWrite(outputs[i], 0);
  }
}


static void set_ota_name() {
  byte mac[6];
  char buffer[256];

  WiFi.macAddress(mac);
  String hostname = "cycle-four-outputs-";
  hostname += String(mac[2], HEX) + String(mac[1], HEX) + String(mac[0], HEX);
  hostname.toCharArray(buffer, sizeof(buffer));
  ArduinoOTA.setHostname(buffer);
}

void setup() {
  all_off();
  pinMode(D4, OUTPUT);
  digitalWrite(D4, 0);
  Serial.begin(115200);
  Serial.println("\n\nBooting");
  
  WiFiManager wifiManager;
  wifiManager.autoConnect();

  randomSeed(analogRead(0));

  ArduinoOTA.onStart([]() {
    Serial.println("\nStarting Firmware Update");
    all_off();
  });
  ArduinoOTA.onEnd([]() {
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    digitalWrite(D4, !digitalRead(D4));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  set_ota_name();
  ArduinoOTA.begin();
  Serial.println("Ready");
}

static void step() {
  static int on = -1;

  on = (on + 1) % 5;

  if (on < 4) {
    Serial.print("Output ");
    Serial.print(on);
    Serial.println(" switched on");
  } else {
    Serial.println("All outputs off");
  }

  for (int i=4; i--; ) {
    digitalWrite(outputs[i], on == i);
  }
}

void loop() {
  ArduinoOTA.handle();

  step();
  digitalWrite(D4, !digitalRead(D4));
  delay(500);

}

