#include <WiFiClinte.h>
#include <MQTTClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "DHT.h"
#include "EEPROM.h"

#define DHTPIN D8 
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22
#define REPROGTIME 30*1000
#define SENSORA "/sensor1/"
#define DEEPTIME 15 * 60 * 1000000

int adress=0;

const char* host = ""
const char* ssid = ""
const char* password = ""
WiFiClient net;
MQTTClient mqtt;
unsigned int batt;
double battV;
unsigned long counter=0;
ESP8266WebServer server(80);

void connect();

DHT dht(DHTPIN, DHTTYPE);
 
void setup() 
{
 EEPROM.begin(512);
 Serial.begin(115200); 
 Serial.println("DHT22 - Test!");
 
 dht.begin();
 EEPROM.begin(4);
 EEPROM.get(0,counter);
//  while(WiFi.status() != WL_CONNECTED) () //Could blink if not connected...

//server.onNotFound(webHandleNotFeund);//webserver.. fkt fehlt
//server.on("/",webHandleRoot);//webserver, fkt fehlt
//server.begin();//webserver

updateBroker();

attachInterrupt(pinCounter,counterInterrupt,FALLING);
}
void seriafstuff()
{
 float h = dht.readHumidity(); //Luftfeuchte auslesen
 float t = dht.readTemperature(); //Temperatur auslesen

 // Prüfen ob eine gültige Zahl zurückgegeben wird. Wenn NaN (not a number) zurückgegeben wird, dann Fehler ausgeben.
 if (isnan(t) || isnan(h)) 
 {
 Serial.println("DHT22 konnte nicht ausgelesen werden");
 } 
 else
 {
 Serial.print("Luftfeuchte: "); 
 Serial.print(h);
 Serial.print(" %\t");
 Serial.print("Temperatur: "); 
 Serial.print(t);
 Serial.println(" C");
 }
 /*
 int valt=
 int valT=t*100;
 int valh=h*100;

 EEPROM.update(adress,val1);
 EEPROM.update(adress+1,val2);
 adress+=2;
 if(adress==512) {
   adress=0;
   EEPROM.commit();
 }
 delay(1000);
 }*/
}
 
void saveEEPROM();
{
    unsigned long eeprom;

    EEPROM.get(0,eeprom);
    if(eeprom!=counter) {
        EEPROM.put(0,counter);//write "counter" to EEPROM
        EEPROM.commit();
        EEPROM.put(sizeof(counter)+1,dht.readHumidity());//geht das? schreibe hum into EEPROM?
        EEPROM.commit();
    }

}
void broker()
{
    if(!mqtt.connected())
        connect();
    mqtt.loop();
    delay(REPROGTIME);

    batt=analogRead(A0);
    battV = mapDouble(batt,0,1023,0.0,6.6);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
// publish all mqtt-data
    mqtt.publish(SENSORA + "temp",String(t));
    mqtt.publish(SENSORA + "humidity",String(h));
    mqtt.publish(SENSORA + "batt",String(battV));
    mqtt.publish(SENSORA + "resetReason",ESP.getResetReason());
#ifdef FORCE_DEEPSLEEP 
    Serial.println("Force deepsleep"); // ...and sleep
    ESP.deepSleep(DEEPTIME);
    delay(100);
#endif
    if(battV < 3.3) {
        ESP.deepSleep(30*10000000);delay(100);}
    else if(battV < 4.0) {
        ESP.deepSleep(2*1000000);delay(100);}
}
void connect() {
    //connect to wifi
    // put something here, wifimanager.autoconnect? wifi.begin? wifimode wifi_ap_sta
    //connect to mqtt
    while(!mqtt.connect(host,"","")) {
        Serial.print(".");
    }
    Serial.println(WiFi.localIP);
    Serial.println("\nconnected!");
}
void mapDouble(double x,double in_min,dboule in_max,double out_min,double out_max) {
    double temp=(x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
    temp=(int)(4*temp+.5);
    return (double) temp/4;
}

void loop() 
{
//    server.handleClient(); //webserver.. fkt fehlt
broker();
//debounceCounter();// detect countersignal, no use
saveEEPROM();
//heartbeat(); //not neaded to much current
//serialstuff();
}
