#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

#include "measure.h"
#include "storage.h"


const char* WIFI_SSID = "FiberCorp WiFi533 2.4Ghz";
const char* WIFI_PASS = "0103061510";

const int BAUD_RATE = 115200;
const int STATUS_PIN = 2;

/***************************** TICKERS **********************************/
const int STATUS_LED_NO_WIFI_TIME = 150;
/************************************************************************/




long t;


unsigned long lastMillisStatusLed = 0;
bool statusLed = false;

void statusLedTickerFunction();
Ticker tickerStatusLed(statusLedTickerFunction,1000);



Measure measure(true);
Storage storage(true);




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

  delay(10);
 
  pinMode(STATUS_PIN, OUTPUT);
  digitalWrite(STATUS_PIN, HIGH);
 
  setupWiFi();

    measure.setup();
    storage.setup();


    tickerStatusLed.start();

}


int i=0;


void parseByte(char *x){
    Serial.print(x[0], HEX);
    Serial.print('|');
    Serial.print(x[1], HEX);
    Serial.print('|');
    Serial.print(x[2], HEX);
    Serial.print('|');
    Serial.println(x[3], HEX);

}

struct test{
    int test;
};

void loop() {
    
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
    

    delay(100);

    test asd;
    asd.test = 0x0D56A326;

    parseByte((char*) &asd);

    delay(2000);

}


