#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
//#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <DNSServer.h>
#include "FS.h"

struct t_measure {
    long time;
    float voltage;
    float current;
    float light;
};

const bool DEBUG = true;

const int BAUD_RATE = 115200;
const int STATUS_PIN = 2;

/***************************** TICKERS **********************************/
const int STATUS_LED_NO_WIFI_TIME = 500;
const int STATUS_LED_DEFAULT_TIME = 2000;
/************************************************************************/

/******************************** WIFI ***********************************/
const char* WIFI_SSID = "Solar Energy Meter";
const char* WIFI_PASS = "11223344";
/************************************************************************/

/******************************** STORAGE ***********************************/
const char* INDEX_FILE = "/index.html";
const char* DATA_FILE = "/data.csv";
const char* FUNCTIONS_FILE = "/functions.js";
const char* STYLE_FILE = "/style.css";
/************************************************************************/

long t;

t_measure currentMeasure;

unsigned long lastMillisStatusLed = 0;
bool statusLed = false;

ESP8266WebServer server(80);

//DS3231 clock;
//RTCDateTime dt;

//void statusLedTickerFunction();
//Ticker tickerStatusLed(statusLedTickerFunction,1000);

/*void statusLedTickerFunction()
{

    if (true)
    {
         statusLed = true;
         tickerStatusLed.interval(STATUS_LED_DEFAULT_TIME);
    }
    else
    {
        statusLed = !statusLed;
        tickerStatusLed.interval(STATUS_LED_NO_WIFI_TIME);
    }

    digitalWrite(STATUS_PIN, !statusLed);
}*/


void resetData(){
    File f = SPIFFS.open(DATA_FILE, "w");
    f.println("Hora,Voltaje,Corriente,Luz");
    f.close();
}

void handleFunctions(){
   File f = SPIFFS.open(FUNCTIONS_FILE,"r");
   server.streamFile(f,"text/javascript");
   f.close();
}

void handleStyle(){
   File f = SPIFFS.open(STYLE_FILE,"r");
   server.streamFile(f,"text/css");
   f.close();
}

void handleStatus(){
  FSInfo fs_info;
  SPIFFS.info(fs_info);

  float porcentage = ((((float)fs_info.totalBytes - (float)fs_info.usedBytes) / (float)fs_info.totalBytes))*100;
  
  server.send(200,"text/plain",String(porcentage)+","+String(currentMeasure.time)+","+String(currentMeasure.voltage)+","+String(currentMeasure.current)+","+String(currentMeasure.light));
}

void handleReset(){
  resetData();
   File f = SPIFFS.open(INDEX_FILE,"r");
   server.streamFile(f,"text/html");
   f.close();
}

void handleData(){
  File f = SPIFFS.open(DATA_FILE, "r");
  server.streamFile(f,"text/csv");
  f.close();
}

void handleRoot(){
   File f = SPIFFS.open(INDEX_FILE,"r");
   server.streamFile(f,"text/html");
   f.close();
}

void setupWifi(){
    WiFi.softAP(WIFI_SSID, WIFI_PASS);
    IPAddress myIP = WiFi.softAPIP();
  
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    
    server.on("/", handleRoot);
    server.on("/reset", handleReset);
    server.on("/data", handleData);
    server.on("/status",handleStatus);
    server.on("/functions.js",handleFunctions);
    server.on("/style.css",handleStyle);
    server.begin();
}

/*void setupClock(){
    clock.begin();
    clock.setDateTime(__DATE__, __TIME__);
}*/


void setup() {
    Serial.begin(BAUD_RATE);
    delay(10);
    Wire.begin();

    //PIN NODEMCU
    pinMode(STATUS_PIN, OUTPUT);
    digitalWrite(STATUS_PIN, LOW);
    
    //setupClock();
    SPIFFS.begin();
    resetData();

    setupWifi();

    //tickerStatusLed.start();

}
/*
long getEpoch(){
     if(DEBUG)
         Serial.println("Main: Getting Time");
     dt = clock.getDateTime();
     if(DEBUG){
         Serial.print("Main: Time = ");
         Serial.print(dt.hour);
         Serial.print(':');
         Serial.print(dt.minute);
         Serial.print(':');
         Serial.println(dt.second);
     }
     return dt.unixtime;
}*/



void storeMeasure(){
    File f = SPIFFS.open(DATA_FILE, "a");
    if (!f){
      Serial.print("file open failed");
      }
    f.print(currentMeasure.time);
    f.print(",");
    f.print(currentMeasure.voltage);
    f.print(",");
    f.print(currentMeasure.current);
    f.print(",");
    f.print(currentMeasure.light);
    f.println("");

    Serial.println(f.size());

    f.close();
}


long previousMillis = 0;
long loopInterval = 1000;
int i=0;

void loop() {
  //tickerStatusLed.update();
  delay(0);
  server.handleClient();
  delay(0);
    
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > loopInterval) { 
    previousMillis = currentMillis;

    currentMeasure.time = 1539489908;
    currentMeasure.current = random(300);
    currentMeasure.voltage = random(300);
    currentMeasure.light = random(300);

    storeMeasure();
  }
}

