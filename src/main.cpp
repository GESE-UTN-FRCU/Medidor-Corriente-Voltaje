#include <Arduino.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <DS3231.h>

#include "measure.h"
#include "storage.h"
#include "transfer.h"


const bool DEBUG = true;

const int BAUD_RATE = 115200;
const int STATUS_PIN = 2;

/***************************** TICKERS **********************************/
const int STATUS_LED_NO_WIFI_TIME = 500;
const int STATUS_LED_DEFAULT_TIME = 2000;
/************************************************************************/




long t;


unsigned long lastMillisStatusLed = 0;
bool statusLed = false;

void statusLedTickerFunction();
Ticker tickerStatusLed(statusLedTickerFunction,1000);
DS3231 clock;
RTCDateTime dt;

StaticJsonBuffer<100> jsonBuffer;


Measure measure(true);
Storage storage(true);
Transfer transfer(true);






void statusLedTickerFunction()
{

    if (transfer.wifiConnected())
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
}


JsonObject& measureToJson(t_measure m){
    jsonBuffer.clear();
    JsonObject &root = jsonBuffer.createObject();

    root.set<long>("t", getEpoch());
    root.set<float>("v", m.voltage);
    root.set<float>("i", m.current);

    return root;
}


int i=0;
void loop() {
    tickerStatusLed.update();

    delay(200);
    measure.loop();
 

    if(i >= 20){
        i=0;
        t_measure m = measure.getMeasure();
        measure.resetMeasure();

        transfer.sendData(measureToJson(m));


    }
    else{
        i++;
    }
    



}


