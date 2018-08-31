#ifndef MEASURE_H
#define MEASURE_H

#include <Arduino.h> //Permite utilizar los comandos de Arduino
 
struct t_measure {
    float current;
    float voltage;
};

class Measure //Definicion de la clase
{
 
    public:

        Measure(bool debug);
    
        void setup();
        void loop();

        void getMeasure();

        void resetMeasure();

    private:
        const float CURRENT_SENSOR_SENSITIVITY = 0.066;
        const float CURRENT_OFFSET_VOLTAGE = 1.545;
        const float ANALOG_VOLTAGE = 3.073;
        const float ANALOG_MAX = 1023.0;


        const int ANALOG_PIN = A0;
        const int CURRENT_SWITCH_PIN = 14;
        const int VOLTAGE_SWITCH_PIN = 12;

        
        bool _debug;

        t_measure measure;
        t_measure measureAux;

        int count;

  

        
 };
 
#endif