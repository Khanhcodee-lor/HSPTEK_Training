#include "IpcProtocol.hpp"
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

class UnixServer {
private:
  int m_serverFd{-1};
  int m_clientFd{-1};

public:
  UnixServer() { ::unlink(SOCKET_PATH); }
  ~UnixServer() { stop(); }

  bool start() {
    m_serverFd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_serverFd < 0) {
      std::cerr << "Error create Socket!\n";
      return false;
    }
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    if (::bind(m_serverFd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) <
        0) {
      std::cerr << "Error bind Socket!\n";
      return false;
    }
    if (::listen(m_serverFd, 5) < 0) {
      std::cerr << "Error listen Socket!\n";
      return false;
    }

    std::cout << "=========================================================\n"
              << "  UNIX DOMAIN SOCKET SERVER C++ ĐANG LẮNG NGHE...\n"
              << "  Socket File Node: " << SOCKET_PATH << "\n"
              << "=========================================================\n";
    return true;
  }
  void run() {
    m_clientFd = ::accept(m_serverFd, nullptr, nullptr);

    if (m_clientFd < 0) {
      std::cerr << "Error accept Socket!\n";
      return;
    }

    std::cout << "Client Connected........\n";
    SensorPacket packet{};
    while (true) {
      ssize_t bytesReceived =
          ::recv(m_clientFd, &packet, sizeof(SensorPacket), 0);
      if (bytesReceived <= 0) {
        std::cerr << "Client disconnected or error receiving!";
        break;
      }
      packet.printInfo();
    }
  }

  void stop() {
    if (m_clientFd > 0) {
      ::close(m_clientFd);
      m_clientFd = -1;
    }
    if (m_serverFd > 0) {
      ::close(m_serverFd);
      m_serverFd = -1;
    }
    ::unlink(SOCKET_PATH);
  }
};

UnixServer g_server;

void signalHandler(int signal){
    std::cout << "\n[Server C++] Received signal (" << signal << "). Cleaning up...\n";
    g_server.stop();
    exit(0);
}

int main(){
    signal(SIGINT, signalHandler);
    if(g_server.start()){
        g_server.run();
    }
}

