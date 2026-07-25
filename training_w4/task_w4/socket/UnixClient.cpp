#include "IpcProtocol.hpp"
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

class UnixClient {
private:
  int m_clientFd{-1};

public:
  ~UnixClient() { disconnect(); }

  bool connectToServer(){
    m_clientFd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if(m_clientFd < 0){
        std::cerr << "Error create Socket!\n";
        return false;
    }
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);
    
    if(::connect(m_clientFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))<0){
        std::cerr << "Error connect Socket!\n";
        return false;
    }
    std::cout << "Connected to Server........\n";
    return true;
  }

  bool sendPacket (const SensorPacket& packet){
    if(m_clientFd <= 0) return false;
    ssize_t bytesSent = ::send(m_clientFd, &packet, sizeof(SensorPacket),0);
    return bytesSent > 0;
  }

  void disconnect(){
    if(m_clientFd > 0){
        ::close(m_clientFd);
        m_clientFd = -1;
    }
  }
};

int main() {
    UnixClient client;
    if (!client.connectToServer()) {
        return 1;
    }
    int count = 0;
    while (true) {
        count++;
        SensorPacket packet{};
        packet.sensorID = 101;
        packet.temperature = 28.5f + (count % 5);
        packet.humidity = 60.0f + (count % 10);
        packet.batteryLevel = 100 - count;
        std::string msg = "Packet C++ send " + std::to_string(count);
        std::strncpy(packet.statusMsg, msg.c_str(), sizeof(packet.statusMsg) - 1);
        std::cout << "[Client C++] Sending packet " << count << " next to Server...\n";
        if (!client.sendPacket(packet)) {
            std::cout << "[Client C++] Sending failed, disconnected.\n";
            break;
        }
        ::sleep(2);
    }
    return 0;
}
