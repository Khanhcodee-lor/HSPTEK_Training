#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H

#include <string>
#include <iostream>

constexpr const char* SOCKET_PATH = "/tmp/embedded_ipc.sock";

struct SensorPacket{
    int sensorID;
    float temperature;
    float humidity;
    int batteryLevel;
    char statusMsg[64];

    void printInfo() {
        std::cout << "Sensor ID: " << sensorID << std::endl;
        std::cout << "Temperature: " << temperature << std::endl;
        std::cout << "Humidity: " << humidity << std::endl;
        std::cout << "Battery Level: " << batteryLevel << std::endl;
        std::cout << "Status Message: " << statusMsg << std::endl;
        std::cout << "------------------------\n";
    }
};


#endif
