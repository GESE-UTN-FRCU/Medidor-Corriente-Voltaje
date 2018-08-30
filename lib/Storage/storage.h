#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h> //Permite utilizar los comandos de Arduino
#include <measure.h> //Permite utilizar la estructura measure
 
class storage //Definicion de la clase
{
    private:
        const float INIT_INDEX = 10;
        const int BUFFER_SIZE = 50;

        bool _debug;

        int index;
 
    public:

        storage(bool debug);

        void setup();

        void store(t_measure measure);

        void retrive(int skip,int limit);

        void remove(int index);

        void resetStorage();

        t_measure measures[BUFFER_SIZE];
        
 };
 
#endif