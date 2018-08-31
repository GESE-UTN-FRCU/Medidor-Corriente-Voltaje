#include "measure.h"




Measure::Measure(bool debug){
    this->_debug=debug;
}


void Measure::setup(){
    pinMode(CURRENT_SWITCH_PIN, OUTPUT);
    pinMode(VOLTAGE_SWITCH_PIN, OUTPUT);

    digitalWrite(CURRENT_SWITCH_PIN,LOW);
    digitalWrite(VOLTAGE_SWITCH_PIN,LOW);
}

void Measure::loop(){
    
    
    digitalWrite(VOLTAGE_SWITCH_PIN,LOW);
    digitalWrite(CURRENT_SWITCH_PIN,HIGH);
    measureAux.current += analogRead(ANALOG_PIN) * (ANALOG_VOLTAGE / ANALOG_MAX);

    digitalWrite(CURRENT_SWITCH_PIN,LOW);
    digitalWrite(VOLTAGE_SWITCH_PIN,HIGH);

    measureAux.voltage += analogRead(ANALOG_PIN) * (ANALOG_VOLTAGE / ANALOG_MAX);
    
    count += 1;
        
}

t_measure Measure::getMeasure(){

        measure.current = measureAux.current / count;
        measure.voltage = measureAux.voltage / count;

        measure.current = (CURRENT_OFFSET_VOLTAGE- measure.current) / CURRENT_SENSOR_SENSITIVITY;
        measure.voltage = measure.voltage * VOLTAGE_MULTIPLY;
    
        if(this->_debug){
            Serial.print("Measure: I (a): ");
            Serial.println(measure.current,5);

            Serial.print("Measure: V (v): ");
            Serial.println(measure.voltage,5);
        }

        return measure;
}

void Measure::resetMeasure(){
        count=0;
        measureAux.current=0;
        measureAux.voltage=0;

}