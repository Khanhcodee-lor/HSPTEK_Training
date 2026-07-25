#include "sensor.h"
#include <stdlib.h>
#include <string.h>

Sensor createSensor(int id, const char *name, float val) {
  Sensor sensor;
  sensor.sensorID = id;
  sensor.sensorName = (char *)malloc(sizeof(char) * strlen(name) + 1);
  strcpy(sensor.sensorName, name);
  sensor.lastValue = val;
  return sensor;
}

void freeSensorContent(Sensor *sensor) {
  if (sensor && sensor->sensorName) {
    free(sensor->sensorName);
    sensor->sensorName = NULL;
  }
}