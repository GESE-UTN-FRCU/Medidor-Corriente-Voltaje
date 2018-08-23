#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>


const char* WIFI_SSID = "FiberCorp WiFi533 2.4Ghz";
const char* WIFI_PASS = "0103061510";

const int BAUD_RATE = 115200;
const int STATUS_PIN = 2;

const int ANALOG_PIN = A0;

/****************************** MQTT ************************************/
const char *MQTT_SERVER = "m14.cloudmqtt.com";
const char *MQTT_USER = "jfdwnjuj";
const char *MQTT_PASSWORD = "re4mRYvxtPvO";
const int MQTT_PORT = 11172;
const char *MQTT_CLIENT_ID = "asds";

const char *LAST_TOPIC = "meters/solarMeter01/last";
/************************************************************************/

/***************************** TICKERS **********************************/
const int STATUS_LED_NO_WIFI_TIME = 150;
const int STATUS_LED_NO_MQTT_TIME = 400;
/************************************************************************/


// Calibration
const float CURRENT_SENSOR_SENSITIVITY = 0.066;
const float CURRENT_OFFSET_VOLTAGE = 2.57088;


//float voltage = 0;
float current = 0;

long t;


unsigned long lastMillisStatusLed = 0;
bool statusLed = false;


WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void statusLedTickerFunction();
Ticker tickerStatusLed(statusLedTickerFunction,1000);







void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived at [");
    Serial.print(topic);
    Serial.print("] of length [");
    Serial.print(length);
    Serial.println("]");
/*
    if (strcmp(topic, REQUEST_TOPIC) == 0)
    {
        parseStatusRequest();
    }
    else if (strcmp(topic, UPDATE_TOPIC) == 0)
    {
        parseUpdate(payload, length);
    }
    else if (strcmp(topic, MODE_TOPIC) == 0)
    {
        parseMode(payload, length);
    }*/
}

void setupWiFi()
{
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}



void reconnect()
{

    Serial.println();
    Serial.println("Attempting MQTT connection");

    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD))
    {
        Serial.println("MQTT Connected");
    }
    else
    {
        Serial.print("failed, rc=");
        Serial.println(mqttClient.state());
    }
}

void statusLedTickerFunction()
{

    if (WiFi.status() == WL_CONNECTED)
    {
        if (mqttClient.connected())
        {
            statusLed = true;
            tickerStatusLed.interval(1);
        }
        else
        {
            statusLed = !statusLed;
            tickerStatusLed.interval(STATUS_LED_NO_MQTT_TIME);
        }
    }
    else
    {
        statusLed = !statusLed;
        tickerStatusLed.interval(STATUS_LED_NO_WIFI_TIME);
    }

    digitalWrite(STATUS_PIN, !statusLed);
}

void setupMQTT()
{
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
}


void setup() {
  Serial.begin(BAUD_RATE);
  delay(10);
 
  pinMode(STATUS_PIN, OUTPUT);
  digitalWrite(STATUS_PIN, HIGH);
 
  setupWiFi();

  setupMQTT();

    tickerStatusLed.start();

}

void medir(){

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

    delay(0);

    if (WiFi.status() == WL_CONNECTED)
    {
        if (!mqttClient.connected())
        {
            Serial.println("MQTT Disconected");
            reconnect();
        }
    }


    delay(0);
    mqttClient.loop();
    delay(0);


/*
    t = millis();
    medir();
    Serial.print("Time: ");
    Serial.println(millis()-t);


    Serial.print("I: ");
    Serial.println(current,5); 
        */


}
 