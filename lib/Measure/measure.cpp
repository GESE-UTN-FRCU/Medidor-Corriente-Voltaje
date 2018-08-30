#include "measure.h"




Measure::Measure(bool debug){
    this->_debug=debug;
}


void Measure::setup(){
    pinMode(CURRENT_SWITCH_PIN, OUTPUT);
    pinMode(VOLTAGE_SWITCH_PIN, OUTPUT);

    digitalWrite(CURRENT_SWITCH_PIN,HIGH);
    digitalWrite(VOLTAGE_SWITCH_PIN,LOW);
}

void Measure::loop(){
    
   // digitalWrite(CURRENT_SWITCH_PIN,HIGH);
  //  delay(1);
    float l = analogRead(ANALOG_PIN) * (ANALOG_VOLTAGE / ANALOG_MAX);

    measureAux.current += l;

    Serial.println(l);
   // digitalWrite(CURRENT_SWITCH_PIN,LOW);

    count += 1;
        
}

void Measure::getMeasure(){

        measure.current = measureAux.current / count;

        measure.current = -(measure.current - CURRENT_OFFSET_VOLTAGE)/ CURRENT_SENSOR_SENSITIVITY;
    
        if(this->_debug){
            Serial.print("Measure: ");
            Serial.println(measure.current,8);
        }
            
        
}

void Measure::resetMeasure(){
        count=0;
        measureAux.current=0;
}