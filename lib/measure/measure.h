#ifndef Measure_h
#define Measure_h

#include "Arduino.h"
#include <ArduinoJson.h>

typedef struct MEASURE_DATA
{
  long time;
  float current;
  float voltage;
};

class Measure
{
public:

  Measure();

  ~Measure();

  void toJson(MEASURE_DATA d, JsonObject &root);

private:


};

#endif