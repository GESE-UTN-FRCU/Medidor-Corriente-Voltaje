#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Wire.h>

#include "measure.h"


const char* WIFI_SSID = "FiberCorp WiFi533 2.4Ghz";
const char* WIFI_PASS = "0103061510";

const int BAUD_RATE = 115200;
const int STATUS_PIN = 2;

//PRUEBA CHICHI 

#define DEVICE 0x57 //this is the device ID from the datasheet of the 24LC256

//in the normal write anything the eeaddress is incrimented after the writing of each byte. The Wire library does this behind the scenes.

template <class T> int eeWrite(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    int i;
    Wire.beginTransmission(DEVICE);
    Wire.write((int)(ee >> 8)); // MSB
    Wire.write((int)(ee & 0xFF)); // LSB
    for (i = 0; i < sizeof(value); i++)
    Wire.write(*p++);
    Wire.endTransmission();
    return i;
}

template <class T> int eeRead(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    int i;
    Wire.beginTransmission(DEVICE);
    Wire.read((int)(ee >> 8)); // MSB
    Wire.read((int)(ee & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(DEVICE,sizeof(value));
    for (i = 0; i < sizeof(value); i++)
    if(Wire.available())
    *p++ = Wire.read();
    return i;
}

struct config
{
long targetLat;
long targetLon;
float fNum;
bool first;
int attempts;
} config;

//PRUEBA CHICHI


/***************************** TICKERS **********************************/
const int STATUS_LED_NO_WIFI_TIME = 150;
/************************************************************************/




long t;


unsigned long lastMillisStatusLed = 0;
bool statusLed = false;

void statusLedTickerFunction();
Ticker tickerStatusLed(statusLedTickerFunction,1000);



Measure measure(true);




void setupWiFi()
{
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}



void statusLedTickerFunction()
{

    if (WiFi.status() == WL_CONNECTED)
    {
         statusLed = true;
         tickerStatusLed.interval(1);
    }
    else
    {
        statusLed = !statusLed;
        tickerStatusLed.interval(STATUS_LED_NO_WIFI_TIME);
    }

    digitalWrite(STATUS_PIN, !statusLed);
}



void setup() {
  Serial.begin(BAUD_RATE);

  //Prueba CHICHI

    config.targetLat = 4957127;
    config.targetLon = 6743421;
    config.first = false;
    config.attempts = 30;
    config.fNum = 2.23;
    eeWrite(0,config);
    delay (30);

  //PRUEBA CHICHI
  delay(10);
 
  pinMode(STATUS_PIN, OUTPUT);
  digitalWrite(STATUS_PIN, HIGH);
 
  setupWiFi();

    measure.setup();


    tickerStatusLed.start();

}


int i=0;

void loop() {

    //PRUEBA CHICHI


    config.targetLat =0;
    config.targetLon = 0;
    config.first = true;
    config.fNum = 0.0;
    config.attempts = 0;

    eeRead(0,config);

    Serial.println(config.targetLat);

    //PRUEBA CHICHI
    
    tickerStatusLed.update();

    measure.loop();

    if(i >= 5){
        i=0;
        measure.getMeasure();

        measure.resetMeasure();
    }
    else{
        i++;
    }
    

    delay(400);

/*
    medir();

    Serial.print("I: ");
    Serial.println(current,5);
*/

}
 