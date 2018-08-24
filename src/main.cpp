#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>


// const char* WIFI_SSID = "FiberCorp WiFi533 2.4Ghz";
// const char* WIFI_PASS = "0103061510";
const char* WIFI_SSID = "MiViejaMula";
const char* WIFI_PASS = "viejamula";

const char* ENDPOINT = "http://10.42.0.1:3000/";

const int BAUD_RATE = 115200;
const int STATUS_PIN = 2;

const int ANALOG_PIN = A0;


/***************************** TICKERS **********************************/
const int STATUS_LED_NO_WIFI_TIME = 150;
/************************************************************************/


// Calibration
const float CURRENT_SENSOR_SENSITIVITY = 0.066;
const float CURRENT_OFFSET_VOLTAGE = 2.57088;


//float voltage = 0;
float current = 0;

long t;

unsigned long lastMillisStatusLed = 0;
bool statusLed = false;

char buffer[1000];

StaticJsonBuffer<1000> jsonBuffer;

void statusLedTickerFunction();
Ticker tickerStatusLed(statusLedTickerFunction,1000);



void setupWiFi()
{
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.reconnect();
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
    
    pinMode(STATUS_PIN, OUTPUT);
    digitalWrite(STATUS_PIN, HIGH);
    
    setupWiFi();

    tickerStatusLed.start();

}

void sendData(){
    jsonBuffer.clear();
    JsonObject &root = jsonBuffer.createObject();

    root.set<float>("i", current);

    root.printTo(buffer);

    HTTPClient http;
    http.begin(ENDPOINT);
    http.addHeader("Content-Type", "application/json");

    int code = http.POST(buffer);

    Serial.println(code);
    http.end();
}


void read(){

  //voltage=0;
  current=0;

  const int LOOP = 100;


    for(int i=0; i< LOOP;i++){

    //float voltageAux=0;
    float currentAux=0;

    
    for(int j=0; j< LOOP;j++){
      //voltageAux += analogRead(A1)*(5.0 / 1023.0);
      currentAux += analogRead(ANALOG_PIN)*(5.0 / 1023.0);
    }
      //voltage += voltageAux / LOOP;
      current += currentAux / LOOP;
    }


    //voltage /= LOOP;
    current /= LOOP;
    current = (current - CURRENT_OFFSET_VOLTAGE)/ CURRENT_SENSOR_SENSITIVITY;
  
}
 
void loop() {
    
    tickerStatusLed.update();

    t = millis();
    read();
    Serial.print("Time: ");
    Serial.println(millis()-t);


    Serial.print("I: ");
    Serial.println(current,5); 

    Serial.print(WiFi.getSleepMode());
        if (WiFi.status() == WL_CONNECTED)
    {
        sendData();
    }

}
 