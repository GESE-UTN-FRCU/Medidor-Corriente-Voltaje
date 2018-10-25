#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "FS.h"


/***********************************************************************/
/****************************** TYPES **********************************/
/***********************************************************************/
struct t_measure {
  long time;
  float voltage;
  float current;
  float light;
};

/***********************************************************************/
/**************************** CONSTANTS ********************************/
/***********************************************************************/

/****************************** PINS ***********************************/
const int STATUS_PIN = 2;
const int ANALOG_PIN = A0;
const int CONTROL_1_PIN = 12;
const int CONTROL_2_PIN = 14;

/******************************* WIFI **********************************/
const char* WIFI_SSID = "Solar Energy Meter";
const char* WIFI_PASS = "11223344";

IPAddress AP_IP(192, 168, 4, 1);
IPAddress NET_MASK(255, 255, 255, 0);

const char* SERVER_URL = "solar.io";
const byte SERVER_PORT = 80;
const byte DNS_PORT = 53;

const char* ROUTE_ROOT = "/";
const char* ROUTE_RESET = "/reset";
const char* ROUTE_DATA = "/data";
const char* ROUTE_STATUS = "/status";
const char* ROUTE_FUNCTIONS = "/functions.js";
const char* ROUTE_STYLE = "/style.css";

/***************************** STORAGE *********************************/
const char* INDEX_FILE = "/index.html";
const char* DATA_FILE = "/data.csv";
const char* FUNCTIONS_FILE = "/functions.js";
const char* STYLE_FILE = "/style.css";

/***************************** GENERAL *********************************/
const bool DEBUG = true;
const bool CLEAR_ON_INIT = true;
const int BAUD_RATE = 115200;

/***********************************************************************/
/**************************** VARIABLES ********************************/
/***********************************************************************/
t_measure currentMeasure;
t_measure lastMeasure;

ESP8266WebServer server(80);
DNSServer dnsServer;

DS3231 clock;
RTCDateTime dt;



/***********************************************************************/
/************************* GENERAL METHODS *****************************/
/***********************************************************************/
void resetMeasure(){
    memset(&currentMeasure, 0, sizeof currentMeasure);
}

void resetData() {
  File f = SPIFFS.open(DATA_FILE, "w");
  f.println("Hora,Voltaje,Corriente,Luz");
  f.close();
}

long getEpoch() {
  dt = clock.getDateTime();
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
  if (DEBUG)
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
  if (DEBUG)
    Serial.println(voltage);
  return voltage;
}

float getLight() {
  float valorMedida;

  digitalWrite(CONTROL_1_PIN, HIGH);
  digitalWrite(CONTROL_2_PIN, LOW);

  delay(10);
  valorMedida = medir() * (3.3 / 1023);
  if (DEBUG)
    Serial.println(valorMedida);
  return valorMedida;
}

void storeMeasure() {
  memcpy(&lastMeasure,&currentMeasure, sizeof currentMeasure);

  File f = SPIFFS.open(DATA_FILE, "a");
  f.printf("%d\tV: %.5f\tI: %.5f,L: %.5f\n",lastMeasure.time,lastMeasure.voltage,lastMeasure.current,lastMeasure.light);
  f.close();
}

/***************************** ROUTES **********************************/
void verifyUrl() {
  if (server.hostHeader() != String(SERVER_URL)) {
    return handleDefault();
  }
}

void handleDefault()
{
  server.sendHeader("Location", "http://" + String(SERVER_URL), true);
  server.send(302, "text/plain", "");
  server.client().stop();
}

void handleRoot() {
  verifyUrl();
  File f = SPIFFS.open(INDEX_FILE, "r");
  server.streamFile(f, "text/html");
  f.close();
}


void handleReset() {
  verifyUrl();
  resetData();
  File f = SPIFFS.open(INDEX_FILE, "r");
  server.streamFile(f, "text/html");
  f.close();
}

void handleData() {
  verifyUrl();
  File f = SPIFFS.open(DATA_FILE, "r");
  server.streamFile(f, "text/csv");
  f.close();
}

void handleStatus() {
  verifyUrl();
  FSInfo fs_info;
  SPIFFS.info(fs_info);

  float porcentage = ((((float)fs_info.totalBytes - (float)fs_info.usedBytes) / (float)fs_info.totalBytes)) * 100;
  server.send(200, "text/plain", String(porcentage) + "," + String(lastMeasure.time) + "," + String(lastMeasure.voltage, 5) + "," + String(lastMeasure.current, 5) + "," + String(lastMeasure.light, 5));
}


void handleFunctions() {
  verifyUrl();
  File f = SPIFFS.open(FUNCTIONS_FILE, "r");
  server.streamFile(f, "text/javascript");
  f.close();
}

void handleStyle() {
  verifyUrl();
  File f = SPIFFS.open(STYLE_FILE, "r");
  server.streamFile(f, "text/css");
  f.close();
}




/***********************************************************************/
/****************************** SETUPS *********************************/
/***********************************************************************/

void setupWifi() {
  WiFi.softAPConfig(AP_IP, AP_IP, NET_MASK);
  WiFi.softAP(WIFI_SSID, WIFI_PASS, 1);

  if (DEBUG) {
    delay(500);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", AP_IP);

  server.on(ROUTE_ROOT, handleRoot);
  server.on(ROUTE_RESET, handleReset);
  server.on(ROUTE_DATA, handleData);
  server.on(ROUTE_STATUS, handleStatus);
  server.on(ROUTE_FUNCTIONS, handleFunctions);
  server.on(ROUTE_STYLE, handleStyle);
  server.onNotFound(handleDefault);

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
  delay(100);
  Serial.begin(BAUD_RATE);
  delay(100);

  Wire.begin();

  setupPin();

  setupClock();

  setupWifi();

  SPIFFS.begin();

  if (CLEAR_ON_INIT)
    resetData();

}

/***********************************************************************/
/******************************* LOOPS *********************************/
/***********************************************************************/

long previousMillis = 0;
long loopInterval = 1;


int ITERACIONES = 100;
int cantidad = 0;

void loop() {
  server.handleClient();
  dnsServer.processNextRequest();


  if (cantidad == ITERACIONES) {
    cantidad = 0;
    currentMeasure.current /= ITERACIONES;
    currentMeasure.voltage /= ITERACIONES;
    currentMeasure.light /= ITERACIONES;
    currentMeasure.time = getEpoch();

    storeMeasure();

    
  }
  else {
    currentMeasure.current += getCurrent();
    currentMeasure.voltage += getVoltage();
    currentMeasure.light += getLight();
    cantidad++;
  }

}
