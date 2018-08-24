#include "Arduino.h"
#include "measure.h"
#include <ArduinoJson.h>


Measure::Measure()
{

}
Measure::~Measure()
{
}

void Measure::toJson(MEASURE_DATA d, JsonObject &root)
{

  root.set("t", d.time);
  root.set("h", d.current);
  root.set("m", d.voltage);

}