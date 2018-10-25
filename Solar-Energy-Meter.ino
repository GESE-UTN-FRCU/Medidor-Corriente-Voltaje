#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <DNSServer.h>
#include "FS.h"


// TYPES
struct t_measure {
  long time;
  float voltage;
  float current;
  float light;
};

const bool DEBUG = true;

const int BAUD_RATE = 115200;

/***************************** PINS **********************************/
const int STATUS_PIN = 2;
const int ANALOG_PIN = A0;
const int CONTROL_1_PIN = 12;
const int CONTROL_2_PIN = 14;
/*********************************************************************/

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
t_measure lastMeasure;

unsigned long lastMillisStatusLed = 0;
bool statusLed = false;

ESP8266WebServer server(80);

DS3231 clock;
RTCDateTime dt;

Ticker tickerStatusLed(statusLedTickerFunction,1000);

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


void resetData() {
  File f = SPIFFS.open(DATA_FILE, "w");
  f.println("Hora,Voltaje,Corriente,Luz");
  f.close();
}

void handleFunctions() {
  File f = SPIFFS.open(FUNCTIONS_FILE, "r");
  server.streamFile(f, "text/javascript");
  f.close();
}

void handleStyle() {
  File f = SPIFFS.open(STYLE_FILE, "r");
  server.streamFile(f, "text/css");
  f.close();
}

void handleStatus() {
  FSInfo fs_info;
  SPIFFS.info(fs_info);

  float porcentage = ((((float)fs_info.totalBytes - (float)fs_info.usedBytes) / (float)fs_info.totalBytes)) * 100;

  server.send(200, "text/plain", String(porcentage) + "," + String(lastMeasure.time) + "," + String(lastMeasure.voltage, 5) + "," + String(lastMeasure.current, 5) + "," + String(lastMeasure.light, 5));
}

void handleReset() {
  resetData();
  File f = SPIFFS.open(INDEX_FILE, "r");
  server.streamFile(f, "text/html");
  f.close();
}

void handleData() {
  File f = SPIFFS.open(DATA_FILE, "r");
  server.streamFile(f, "text/csv");
  f.close();
}

void handleRoot() {
  File f = SPIFFS.open(INDEX_FILE, "r");
  server.streamFile(f, "text/html");
  f.close();
}

void setupWifi() {
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  server.on("/data", handleData);
  server.on("/status", handleStatus);
  server.on("/functions.js", handleFunctions);
  server.on("/style.css", handleStyle);
  server.begin();
}

void setupClock() {
  clock.begin();
  clock.setDateTime(__DATE__, __TIME__);
}

void setupPin() {
  pinMode(STATUS_PIN, OUTPUT);
  digitalWrite(STATUS_PIN, LOW);

  pinMode(CONTROL_1_PIN, OUTPUT);
  digitalWrite(CONTROL_1_PIN, LOW);
  pinMode(CONTROL_2_PIN, OUTPUT);
  digitalWrite(CONTROL_2_PIN, LOW);

}

void setup() {
  delay(500);
  Serial.begin(BAUD_RATE);
  delay(500);


  Wire.begin();

  setupPin();
  setupClock();
  setupWifi();

  SPIFFS.begin();
  resetData();



  tickerStatusLed.start();

}

long getEpoch() {
  if (DEBUG)
    Serial.println("Main: Getting Time");
  dt = clock.getDateTime();
  if (DEBUG) {
    Serial.print("Main: Time = ");
    Serial.print(dt.hour);
    Serial.print(':');
    Serial.print(dt.minute);
    Serial.print(':');
    Serial.println(dt.second);
  }
  return dt.unixtime;
}

float medir() {
  float medida = 0;
  int muestras = 1000;
  for (int i = 1; muestras >= i; i++) {
    medida = medida + analogRead(ANALOG_PIN);
    yield();
  }
  medida = (float)(medida / muestras);
  return medida;
}

float getCurrent() {
  float current;

  digitalWrite(CONTROL_1_PIN, LOW);
  digitalWrite(CONTROL_2_PIN, LOW);

  delay(10);
  current = medir() * (3.3 / 1023.0);
  Serial.println(current);
  //current = (-1) * (current - 2.69) / (0.185);

  return current;
}

float getVoltage() {
  float voltage;

  digitalWrite(CONTROL_1_PIN, LOW);
  digitalWrite(CONTROL_2_PIN, HIGH);

  delay(10);
  voltage = medir() * (3.3 / 1023.0);
  Serial.println(voltage);
  return voltage;
}

float getLight() {
  float valorMedida;

  digitalWrite(CONTROL_1_PIN, HIGH);
  digitalWrite(CONTROL_2_PIN, LOW);

  delay(10);
  valorMedida = medir() * (3.3 / 1023);
  Serial.println(valorMedida);
  return valorMedida;
}

void storeMeasure() {
  File f = SPIFFS.open(DATA_FILE, "a");
  if (!f) {
    Serial.print("file open failed");
  }
  lastMeasure.time = currentMeasure.time;
  lastMeasure.voltage = currentMeasure.voltage;
  lastMeasure.current = currentMeasure.current;
  lastMeasure.light = currentMeasure.light;


  f.print(lastMeasure.time);
  f.print(",");
  f.print(lastMeasure.voltage, 5);
  f.print(",");
  f.print(lastMeasure.current, 5);
  f.print(",");
  f.print(lastMeasure.light, 5);
  f.println("");

  //Serial.println(f.size());

  f.close();
}


long previousMillis = 0;
long loopInterval = 1;


int ITERACIONES = 100;
int cantidad = 0;


void loop() {
  //tickerStatusLed.update();
  server.handleClient();

  //unsigned long currentMillis = millis();
  //if (currentMillis - previousMillis > loopInterval) {
  //previousMillis = currentMillis;

  if (cantidad == ITERACIONES) {
    cantidad = 0;
    currentMeasure.current /= ITERACIONES;
    currentMeasure.voltage /= ITERACIONES;
    currentMeasure.light /= ITERACIONES;
    currentMeasure.time = getEpoch();
    
    storeMeasure();

    currentMeasure.time = 0;
    currentMeasure.current = 0;
    currentMeasure.voltage = 0;
    currentMeasure.light = 0;

  }
  else {
    currentMeasure.current += getCurrent();
    currentMeasure.voltage += getVoltage();
    currentMeasure.light += getLight();
    cantidad++;
  }




  //}
}
