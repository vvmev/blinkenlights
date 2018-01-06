#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Ticker.h>

Ticker tickStep;

#define NAME "schotterzwerg"

#define NOUTPUTS (sizeof(outputs)/sizeof(*outputs))
static int outputs[] = { D7, D6, D2, D1 };

/*
 * Outputs:
 *   1r  3w  2r
 *   4w
 * 
 */

#define NASPECTS (sizeof(aspects)/sizeof(*aspects))
static char aspects[][NOUTPUTS] = {
  { 1, 1, 0, 0}, /* Hp0  */
  { 0, 0, 1, 1}, /* Sh1  */
  { 1, 1, 0, 0}, /* Hp0  */
  { 0, 0, 1, 0}, /* Ke   */
};

ESP8266WebServer webserver(80);

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
  String hostname = NAME "-";
  hostname += String(mac[3], HEX) + "-" + String(mac[4], HEX) + "-" + String(mac[5], HEX);
  hostname.toCharArray(buffer, sizeof(buffer));
  ArduinoOTA.setHostname(buffer);
}

void handleRoot() {
  String message = "This is " NAME "\n";
  message += "MAC address: " + String(WiFi.macAddress()) + "\n";
  message += "NOUTPUTS = " + String(NOUTPUTS) + "\n";
  message += "NASPECTS = " + String(NASPECTS) + "\n";
  webserver.send(200, "text/plain", message);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webserver.uri();
  message += "\nMethod: ";
  message += (webserver.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webserver.args();
  message += "\n";
  for (uint8_t i=0; i<webserver.args(); i++){
    message += " " + webserver.argName(i) + ": " + webserver.arg(i) + "\n";
  }
  webserver.send(404, "text/plain", message);
}


static void step() {
  static int aspect = 0;

  for (int i=NOUTPUTS; i--; ) {
    digitalWrite(outputs[i], aspects[aspect][i]);
  }
  aspect = (aspect + 1) % NASPECTS;
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
  
  webserver.on("/", handleRoot);
  webserver.on("/inline", [](){
    webserver.send(200, "text/plain", "this works as well");
  });
  webserver.onNotFound(handleNotFound);
  webserver.begin();
  
  tickStep.attach(10.0, step);

  Serial.println("Ready");
  Serial.println(NASPECTS);
}


void loop() {
  ArduinoOTA.handle();
  webserver.handleClient();

  //digitalWrite(D4, !digitalRead(D4));

  delay(100);
}

