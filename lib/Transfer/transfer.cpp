#include "transfer.h"



Transfer::Transfer(ESP8266WebServer* server, bool debug){
    this->_debug = debug;
    this->server = server;
}

void Transfer::handleRoot(){
   server->send(200, "text/html", "<h1>You are connected</h1>");
}

void Transfer::setup(){
    
}

void Transfer::enable(){
    WiFi.mode(WIFI_AP);
}

void Transfer::disable(){
    WiFi.mode(WIFI_OFF);
}

void Transfer::loop(){
    server->handleClient();
}