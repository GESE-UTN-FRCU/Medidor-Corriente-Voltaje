#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h> //Permite utilizar los comandos de Arduino
#include <measure.h> //Permite utilizar la estructura measure
#include <Wire.h>
 
class Storage //Definicion de la clase
{
    private:
        const float INIT_INDEX = 10;
        const int BUFFER_SIZE = 50;
        const int DEVICE = 0x57;
        const int struct_size = sizeof(t_measure);

        bool _debug;

        int index;
 
    public:

        Storage(bool debug);

        void setup();

        void store(t_measure measure);

        void retriveLast();

        void removeLast();

        void resetStorage();

        t_measure lastReadedMeasure;
 };
 
#endif