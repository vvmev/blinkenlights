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

int brightness = 512; // 0 is full brightness; any other value PWM up to PWMRANGE

int aspect = 0;

int autoStep = 1;


ESP8266WebServer webserver(80);

static void all_off() {
  for (int i=4; i--; ) {
    pinMode(outputs[i], OUTPUT);
    digitalWrite(outputs[i], 0);
  }
}


static void setOutputs() {
  for (int i=NOUTPUTS; i--; ) {
    if (brightness == 0) {
      analogWrite(outputs[i], 0);
      digitalWrite(outputs[i], aspects[aspect][i]);
    } else {
      analogWrite(outputs[i], aspects[aspect][i] ? brightness : 0);
    }
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


String button(String name, int current, int value, String label) {
  return "<a href='/set?" + name + "=" + String(value) + "' class='btn" + (current == value ? " act" : "") + "'>" + label + "</a> ";
}

void handleRoot() {
  String message = "<html><head><title> " NAME "</title>\n";
  message += "<link rel='stylesheet' type='text/css' href='m.css'>";
  message += "<meta name='viewport' content='initial-scale=1.0'>";
  message += "</head><body>";
  message += "<h1>" NAME "</h1>";

  message += "<h2>Weiterschalten</h2><p>";
  message += button("s", autoStep, 0, "Manuell");
  message += button("s", autoStep, 1, "Auto") + "</p>";

  message += "<h2>Signalbegriff</h2><p>";
  message += button("a", aspect, 0, "Hp0");
  message += button("a", aspect, 1, "Sh1");
  message += button("a", aspect, 3, "Ke") + "</p>";

  message += "<h2>Helligkeit</h2><p>";
  message += button("b", brightness, 256, "33%");
  message += button("b", brightness, 512, "50%");
  message += button("b", brightness, 0, "100%") + "</p>";

  message += "<pre>MAC address: " + String(WiFi.macAddress()) + "\n";
  message += "NOUTPUTS = " + String(NOUTPUTS) + "\n";
  message += "NASPECTS = " + String(NASPECTS) + "\n";
  message += "aspect = " + String(aspect) + "\n";
  message += "brightness = " + String(brightness) + "\n";
  message += "autoStep = " + String(autoStep) + "\n</pre>";

  message += "</body></html>";
  webserver.send(200, "text/html", message);
}

void handleCss() {
  String message = "";
  message += "body {font:  10pt 'Arial'; }";
  message += "h2 {font: bold 12pt; }";
  message += "a.btn { width: 6em;";
  message += "background-color: #888; color: white; padding: 10px; text-align: center; text-decoration: none; ";
  message += "display: inline-block; font: bold 10pt 'Arial'; margin: 4px 2px; cursor: pointer; border-radius: 12px;}";
  message += "a.act { background-color: #4CAF50; }";
  webserver.send(200, "text/css", message);
}


void handleSet() {
  String a;
  int n;
  if (webserver.hasArg("a")) {
    a = webserver.arg("a");
    n = a.toInt();
    if (n >= 0 && n < NASPECTS)
      aspect = n;
  }
  if (webserver.hasArg("b")) {
    a = webserver.arg("b");
    n = a.toInt();
    if (n >= 0 && n < PWMRANGE)
      brightness = n;
  }
  if (webserver.hasArg("s")) {
    a = webserver.arg("s");
    n = a.toInt();
    autoStep = n != 0;
  }
  setOutputs();
  webserver.sendHeader("Location", String("/"), true);
  webserver.send ( 302, "text/plain", "");
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
  setOutputs();
  if (autoStep)
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
  webserver.on("/m.css", handleCss);
  webserver.on("/set", handleSet);
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
