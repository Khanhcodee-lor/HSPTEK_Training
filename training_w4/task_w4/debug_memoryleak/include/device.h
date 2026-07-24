#ifndef DEVICE_H
#define DEVICE_H

#include "sensor.h"

typedef struct Device {
    int deviceID;
    char* deviceName;
    char* ipAddress;
    Sensor* sensorList;
    int sensorCount;    
    struct Device* next;
}Device;

Device createDevice(int deviceID, char* deviceName, char* ipAddress);
void addSensorToDevice(Device* device, int sensorID, char* sensorName, float lastValue);
void addDeviceToList(Device** headRef, Device* device);
void removeDeviceById(Device** headRef, int deviceID);
void freeDevice(Device* headRef);

#endif