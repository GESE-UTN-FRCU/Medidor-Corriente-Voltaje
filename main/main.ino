#include <Arduino.h>

// PINOUT
const int CURRENT_SENSOR_PIN = A0;
const int VOLTAGE_SENSOR_PIN = A1;

// CONSTANTS
const float CURRENT_SENSOR_SENS = 0.66; // 0.185 - 0.100 - 0.66 segun el modelo

// VARIABLES
float voltage,current;

float readCurrent(){
    float v = analogRead(CURRENT_SENSOR_PIN)*(5.0 / 1023.0);
    return (v-2.5)/ CURRENT_SENSOR_SENS;
}

float readVoltage(){
    float v = analogRead(VOLTAGE_SENSOR_PIN)*(5.0 / 1023);
    return v;
}


void setup() {
    Serial.begin(9600);
}

void loop() {
   
  current = readCurrent();
  voltage = readVoltage();

    Serial.print("V: ");
    Serial.print(voltage,3);
    Serial.print("  I: ");
    Serial.println(current,3); 
  
  delay(1000);     
}

