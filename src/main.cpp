#include <Arduino.h>
#include <Ticker.h>
#include <DS3231.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
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
const char* DATA_DOWNLOAD_URI = "/datos.csv";
/************************************************************************/

/******************************** STORAGE ***********************************/
const char* DATA_FILE = "/data.csv";
/************************************************************************/

long t;

t_measure currentMeasure;

unsigned long lastMillisStatusLed = 0;
bool statusLed = false;

ESP8266WebServer server(80);

void statusLedTickerFunction();
Ticker tickerStatusLed(statusLedTickerFunction,1000);

// DS3231 clock;
// RTCDateTime dt;




void statusLedTickerFunction()
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
}

void resetData(){
    File f = SPIFFS.open(DATA_FILE, "w");
    f.println("Hora,Voltaje,Corriente,Luz");
    f.close();
}

void handleRoot(){
   server.send(200, "text/html", "<h1>Running</h1>");
}

void setupWifi(){
    WiFi.softAP(WIFI_SSID, WIFI_PASS);
    IPAddress myIP = WiFi.softAPIP();
  
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    

    server.on("/", handleRoot);
    server.serveStatic(DATA_DOWNLOAD_URI, SPIFFS, DATA_FILE);
    server.begin();
}

void setupClock(){
    // clock.begin();
    // clock.setDateTime(__DATE__, __TIME__);
}


void setup() {
    Serial.begin(BAUD_RATE);
    delay(10);

    pinMode(STATUS_PIN, OUTPUT);
    digitalWrite(STATUS_PIN, LOW);

    //setupClock();


    SPIFFS.begin();
    resetData();

    setupWifi();

    tickerStatusLed.start();

}

// long getEpoch(){
//     if(DEBUG)
//         Serial.println("Main: Getting Time");
//     dt = clock.getDateTime();
//     if(DEBUG){
//         Serial.print("Main: Time = ");
//         Serial.print(dt.hour);
//         Serial.print(':');
//         Serial.print(dt.minute);
//         Serial.print(':');
//         Serial.println(dt.second);
//     }
//     return dt.unixtime;
// }



void storeMeasure(){
    File f = SPIFFS.open(DATA_FILE, "a");
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



int i=0;
void loop() {
    tickerStatusLed.update();
    delay(0);
    server.handleClient();
    delay(0);
    

 
    if(i >= 50){
        i=0;
        currentMeasure.time = random(300);
        currentMeasure.current = random(300);
        currentMeasure.voltage = random(300);
        currentMeasure.light = random(300);

        storeMeasure();
    }
    else{
        i++;
    }

    delay(100);
   
    


}


