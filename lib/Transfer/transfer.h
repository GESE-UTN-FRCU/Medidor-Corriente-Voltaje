#ifndef TRANSFER_H
#define TRANSFER_H

#include <Arduino.h> //Permite utilizar los comandos de Arduino
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
 
class Transfer //Definicion de la clase
{

    public:
        Transfer(ESP8266WebServer* server, bool debug);

        void setup();

        void loop();

        void enable();
        
        void disable();


    private:



        ESP8266WebServer* server;
    
        //const ESP8266WebServer server =  ESP8266WebServer(80);

        bool _debug;

        char buffer[100];    

        void handleRoot();
        
 };
 
 

#endif