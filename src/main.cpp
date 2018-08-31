#include <Arduino.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <DS3231.h>

#include "measure.h"
#include "storage.h"
#include "transfer.h"



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
DS3231 clock;
RTCDateTime dt;


Measure measure(true);
Storage storage(true);
Transfer transfer(true);






void statusLedTickerFunction()
{

    if (transfer.wifiConnected())
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

    clock.begin();
    clock.setDateTime(__DATE__, __TIME__);

    pinMode(STATUS_PIN, OUTPUT);
    digitalWrite(STATUS_PIN, HIGH);

    transfer.setup();
    measure.setup();
    storage.setup();

    tickerStatusLed.start();
}

long getEpoch(){
    if(debug)
        Serial.println("Main: Getting Time");
    dt = clock.getDateTime();
    if(debug){
        Serial.println("Main: Time = ");
        Serial.println(dt.unixtime);
    }
    return dt.unixtime:
}


int i=0;
void loop() {
    
    tickerStatusLed.update();

    measure.loop();

    if(i >= 1000){
        i=0;
        measure.getMeasure();

        measure.resetMeasure();
    }
    else{
        i++;
    }
    



}


