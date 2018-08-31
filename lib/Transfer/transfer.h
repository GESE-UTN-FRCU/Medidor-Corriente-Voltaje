#ifndef TRANSFER_H
#define TRANSFER_H

#include <Arduino.h> //Permite utilizar los comandos de Arduino

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


 
class Transfer //Definicion de la clase
{

    public:
        Transfer(bool debug);

        void setup();

        void loop();

        bool sendData(JsonObject &root);

        bool wifiConnected();


    private:
        const char* WIFI_SSID = "FiberCorp WiFi533 2.4Ghz";
        const char* WIFI_PASS = "0103061510";
        const char* ENDPOINT = "192.168.0.13:3000/";

        bool _debug;

        char buffer[100];

        
    
 };
 
#endif