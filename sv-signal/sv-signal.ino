#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>

//                       rot 12V
#define lamp D5       // weiß pin3 Stecker Sv
#define lampa D6      // grün pin4 Stecker Sv
#define relaisL D7    // blau pin1 Stecker Sv
#define relaisR D8    // gelb pin2 Stecker Sv
#define zp9 D0        // gelb pin2 Stecker Zp
#define zp10 D1       // blau pin1 Stecker Zp
#define fahrsp D2     //
#define inp1 D3       //
#define inp2 D4       //
#define zeit 2000
void state(int wahla);
int wahl;

// 
static void set_ota_name() {
  byte mac[6];
  char buffer[256];

  WiFi.macAddress(mac);
  String hostname = "SV-Signal";
  hostname += String(mac[2], HEX) + String(mac[1], HEX) + String(mac[0], HEX);
  hostname.toCharArray(buffer, sizeof(buffer));
  ArduinoOTA.setHostname(buffer);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nBooting");
  
  WiFiManager wifiManager;
  wifiManager.autoConnect();

  randomSeed(analogRead(0));

  ArduinoOTA.onStart([]() {
    Serial.println("\nStarting Firmware Update");
  });
  ArduinoOTA.onEnd([]() {
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//    digitalWrite(D4, !digitalRead(D4));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
//  set_ota_name();
  ArduinoOTA.begin();
  Serial.println("Ready");

// all pin as out-/input and output at 0
  pinMode(lamp, OUTPUT);
  digitalWrite(lamp,0);
  pinMode(lampa, OUTPUT);
  digitalWrite(lampa,0);
  pinMode(relaisL, OUTPUT);
  digitalWrite(relaisL,0);
  pinMode(relaisR, OUTPUT);
  digitalWrite(relaisR,0);
  pinMode(zp9, OUTPUT);
  digitalWrite(zp9,0);
  pinMode(zp10, OUTPUT);
  digitalWrite(zp10,0);
  pinMode(fahrsp, OUTPUT);
  digitalWrite(fahrsp,0);
  pinMode(inp1, OUTPUT);
  digitalWrite(inp1,0);
  pinMode(inp2, OUTPUT);
  digitalWrite(inp2,0);
  int wahl=0;

  // Wifi-Einstellungen
//  ArduinoOTA.setPassword((const char *)"123");
}

// the loop function runs over and over again forever
void loop() {
  ArduinoOTA.handle();
  wahl++;
  if(wahl==10)
    wahl=0;
  delay(zeit);
  state(wahl);
}

void state(int wahla)
{
  switch(wahla) {
    case 0: // Sv0
            digitalWrite(lamp,1);
            digitalWrite(lampa,0);
            digitalWrite(relaisL,0);
            digitalWrite(relaisR,0);
            digitalWrite(zp9,0);
            digitalWrite(zp10,0);
            break;
    case 1: // Sv1
            digitalWrite(lamp,1);
            digitalWrite(lampa,0);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,1);
            digitalWrite(zp9,0);
            digitalWrite(zp10,0);
            break;
    case 2: // Sv1v
            digitalWrite(lampa,1);
            delay(300);
            digitalWrite(lamp,1);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,1);
            digitalWrite(zp9,0);
            digitalWrite(zp10,0);
            break;
    case 3: // Sv2
            digitalWrite(lamp,1);
            digitalWrite(lampa,0);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,0);
            digitalWrite(zp9,0);
            digitalWrite(zp10,0);
            break;
    case 4: // Sv1+Zp10
            digitalWrite(lamp,1);
            digitalWrite(lampa,0);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,1);
            digitalWrite(zp9,0);
            digitalWrite(zp10,1);
            break;
    case 5: // Sv2+Zp9
            digitalWrite(lamp,1);
            digitalWrite(lampa,0);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,1);
            digitalWrite(zp9,1);
            digitalWrite(zp10,0);
            break;
    case 6: // Sv1v+Zp9
            digitalWrite(lampa,1);
            delay(300);          
            digitalWrite(lamp,1);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,1);
            digitalWrite(zp9,1);
            digitalWrite(zp10,0);
            break;
    case 7: // Sv2v+Zp9
            digitalWrite(lamp,1);
            digitalWrite(lampa,1);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,0);
            digitalWrite(zp9,1);
            digitalWrite(zp10,0);
            break;
    case 8: // Sv1v+Zp10
            digitalWrite(lamp,1);
            digitalWrite(lampa,1);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,1);
            digitalWrite(zp9,0);
            digitalWrite(zp10,1);
            break;
    case 9: // Sv2v+Zp10
            digitalWrite(lamp,1);
            digitalWrite(lampa,1);
            digitalWrite(relaisL,1);
            digitalWrite(relaisR,0);
            digitalWrite(zp9,0);
            digitalWrite(zp10,1);
            break;
  }
}


