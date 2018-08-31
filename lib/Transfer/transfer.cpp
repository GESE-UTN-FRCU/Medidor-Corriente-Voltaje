#include "transfer.h"





Transfer::Transfer(bool debug){
    this->_debug = debug;
}


void Transfer::setup(){
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void Transfer::loop(){

}

bool Transfer::sendData(JsonObject &root){
    if(_debug)
        Serial.println("Transfer: Sending data");
    root.printTo(buffer);

    HTTPClient http;
    http.begin(ENDPOINT);
    http.addHeader("Content-Type", "application/json");
    int code = http.POST(buffer);
    http.end();

    if(_debug){
        Serial.print("Transfer: Status Code = ");
        Serial.println(code);
    }
    return code == 200;
}

bool Transfer::wifiConnected(){
    return WiFi.status() == WL_CONNECTED;
}