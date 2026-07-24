#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sensor.h"

Sensor createSensor (int id, const char* name, float val){
    Sensor s;
    s.sensorID = id;
    s.sensorName = (char*) malloc(sizeof(char)*strlen(name)+1);
    strcpy(s.sensorName, name);
    s.lastValue = val;
    return s;
}

void freeSensorContent(Sensor* sensor){
    if (s && s->sensor_name) {
        free(s->sensor_name);
        s->sensor_name = NULL;
    }
}