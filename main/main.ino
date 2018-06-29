#include <Arduino.h>
#include <DS3231_Simple.h>
#include <Wire.h>

// DEVICEs
DS3231_Simple Clock;

// PINOUT
const int CURRENT_SENSOR_PIN = A0;
const int VOLTAGE_SENSOR_PIN = A1;

// CONSTANTS
const float CURRENT_SENSOR_SENS = 0.066; // 0.185 - 0.100 - 0.066 segun el modelo

// VARIABLES
float voltage,current,lastCurrent,lastVoltage;

float readCurrent(){
    float v = analogRead(CURRENT_SENSOR_PIN)*(5.0 / 1023.0);
    return (v-2.5035)/ CURRENT_SENSOR_SENS;
}

float readVoltage(){
    float v = analogRead(VOLTAGE_SENSOR_PIN)*(5.0 / 1023)*(5.91);
    return v;
}

void readSensors(int n_muestras){
  
  lastCurrent = 0;
  lastVoltage = 0;

  for(int i=0;i<n_muestras;i++)
  {
    lastCurrent = lastCurrent+readVoltage();
    lastVoltage = lastVoltage+readVoltage();  
  }

  lastCurrent = lastCurrent / n_muestras;
  lastVoltage = lastVoltage / n_muestras;
}

void setup() {
    Wire.begin();
    Serial.begin(9600);

}

void loop() {
   
  current = readCurrent();
  voltage = readVoltage();

    //PRINT VOLTAGE AND CURRENT
    Serial.print("V: ");
    Serial.print(voltage,4);
    Serial.print("  I: ");
    Serial.println(current,4);

    //PRINT DATE
    Clock.printDateTo_YMD(Serial);
    Serial.print(' ');
    Clock.printTimeTo_HMS(Serial);
    Serial.println();
    
  
  delay(1000);     
}

