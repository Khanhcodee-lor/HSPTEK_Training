#ifndef SENSOR_H
#define SENSOR_H

typedef struct Sensor{
    int sensorID;
    char* sensorName;
    float lastValue;
} Sensor;

Sensor createSensor(int sensorID, char* sensorName, float lastValue);
void freeSensorContent(Sensor* sensor);

#endif