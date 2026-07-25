#include "device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Device *createDevice(int deviceID, char *deviceName, char *ipAddress) {
  Device *device = (Device *)malloc(sizeof(Device));
  device->deviceID = deviceID;

  device->deviceName = (char *)malloc(sizeof(char) * strlen(deviceName) + 1);
  strcpy(device->deviceName, deviceName);

  device->ipAddress = (char *)malloc(sizeof(char) * strlen(ipAddress) + 1);
  strcpy(device->ipAddress, ipAddress);

  device->sensorList = NULL;
  device->sensorCount = 0;
  device->next = NULL;
  return device;
}

void addSensorToDevice(Device *device, int sensorID, char *sensorName,
                       float lastValue) {
  if (device == NULL)
    return;
  device->sensorCount++;
  device->sensorList = (Sensor *)realloc(device->sensorList,
                                         device->sensorCount * sizeof(Sensor));
  int idx = device->sensorCount - 1;
  device->sensorList[idx] = createSensor(sensorID, sensorName, lastValue);
}

void addDeviceToList(Device **headRef, Device *device) {
  if (*headRef == NULL) {
    *headRef = device;
  } else {
    Device *current = *headRef;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = device;
  }
}
void removeDeviceById(Device **headRef, int deviceID) {
  if (headRef == NULL)
    return;
  Device *current = *headRef;
  Device *prev = NULL;
  while (current != NULL && current->deviceID != deviceID) {
    prev = current;
    current = current->next;
  }
  if (current == NULL)
    return;
  if (prev == NULL) {
    *headRef = current->next;
  } else {
    prev->next = current->next;
  }
  // Deep free toàn bộ bộ nhớ bên trong node
  if (current->deviceName)
    free(current->deviceName);
  if (current->ipAddress)
    free(current->ipAddress);
  if (current->sensorList) {
    for (int i = 0; i < current->sensorCount; i++) {
      freeSensorContent(&(current->sensorList[i]));
    }
    free(current->sensorList);
  }
  free(current);
}

void freeDeviceContent(Device *device) {
  if (device == NULL)
    return;
  if (device->deviceName)
    free(device->deviceName);
  if (device->ipAddress)
    free(device->ipAddress);
  if (device->sensorList) {
    for (int i = 0; i < device->sensorCount; i++) {
      freeSensorContent(&(device->sensorList[i]));
    }
    free(device->sensorList);
  }
}

void freeDevice(Device *head) {
  Device *current = head;
  Device *next = NULL;
  while (current != NULL) {
    next = current->next;
    freeDeviceContent(current);
    free(current);
    current = next;
  }
}

void printDeviceList(Device *head) {
  Device *current = head;
  if (current == NULL) {
    printf("No devices found\n");
    return;
  }
  while (current != NULL) {
    printf("Device ID: %d, Name: %s, IP: %s, Sensor Count: %d\n",
           current->deviceID, current->deviceName, current->ipAddress,
           current->sensorCount);
    current = current->next;
  }
}
