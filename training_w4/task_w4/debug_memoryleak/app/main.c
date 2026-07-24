#include <stdio.h>
#include "device.h"

int main(){
    printf("UNIFIED DEVICE MANAGER");
    Device* head = NULL

    Device* d1 = createDevice(101, "Gateway", "192.168.1.50");
    Device* d2 = createDevice(102, "SensorNode", "192.168.1.51");
    Device* d3 = createDevice(103, "Camera", "192.168.1.52");

    addDeviceToList(&head, d1);
    addDeviceToList(&head, d2);
    addDeviceToList(&head, d3);

    addSensorToDevice(d1, 1, "temp", 25.5f);
    addSensorToDevice(d1, 2, "hum", 45.3f);
    addSensorToDevice(d2, 3, "volt", 12.0f);

    printf("\n=== DEVICE LIST ===\n");
    printDeviceList(head);

    printf("\n=== REMOVE DEVICE 102 ===\n");
    removeDeviceById(&head, 102);
    printDeviceList(head);

    printf("\n=== REMOVE DEVICE 101 ===\n");
    removeDeviceById(&head, 101);
    printDeviceList(head);

    printf("\n=== FREE ALL ===\n");
    freeDevice(head);
    return 0;
}