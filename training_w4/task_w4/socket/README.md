# Báo Cáo Task: Lập Trình IPC Với Unix Domain Socket (Server/Client C++)

## 1. Tổng Quan & Mục Tiêu

Báo cáo này mô tả chi tiết kiến thức lý thuyết, kiến trúc ứng dụng và kết quả thực hành bài tập **Unix Domain Socket (IPC)** thuộc học phần *Embedded Linux cơ bản*.

### Mục tiêu chính:
- **Xây dựng hệ thống giao tiếp giữa các tiến trình (Inter-Process Communication - IPC)** trên cùng một hệ thống Linux sử dụng mô hình Unix Domain Socket (AF_UNIX) theo dạng Stream (`SOCK_STREAM`).
- **Thiết kế giao thức truyền tải dữ liệu (IPC Protocol)**: Đóng gói dữ liệu cảm biến dạng Binary Struct (`SensorPacket`) chứa ID, nhiệt độ, độ ẩm, dung lượng pin và thông điệp trạng thái.
- **Hiện thực mô hình Server - Client chuẩn OOP trong C++**:
  - **Server (`UnixServer.cpp`)**: Đảm nhận tạo file socket `/tmp/embedded_ipc.sock`, lắng nghe kết nối, nhận gói tin từ Client và in thông tin hiển thị.
  - **Client (`UnixClient.cpp`)**: Kết nối tới Server, định kỳ đóng gói và gửi thông số cảm biến mới mỗi 2 giây.
- **Quản lý vòng đời & Dọn dẹp tài nguyên**: Bắt tín hiệu ngắt `SIGINT` (`Ctrl+C`) để chủ động đóng các Socket File Descriptor (`close`) và xóa bỏ file node socket (`unlink`), tránh lỗi `Address already in use` trong lần khởi chạy tiếp theo.

---

## 2. Cấu Trúc Dự Án

```text
socket/
├── IpcProtocol.hpp   # Định nghĩa giao thức IPC, đường dẫn Socket và cấu trúc SensorPacket
├── UnixServer.cpp    # Mã nguồn phía Server (Khởi tạo, Lắng nghe, Nhận dữ liệu & Xử lý Signal)
├── UnixClient.cpp    # Mã nguồn phía Client (Kết nối, Đóng gói dữ liệu & Gửi định kỳ)
├── server_cpp        # File thực thi phía Server sau khi biên dịch
├── client_cpp        # File thực thi phía Client sau khi biên dịch
└── README.md         # Báo cáo chi tiết bài tập
```

---

## 3. Kiến Thức Kỹ Thuật & Cấu Trúc Mã Nguồn

### 3.1. Giao Thức IPC & Định Dạng Gói Tin ([IpcProtocol.hpp](file:///home/khanh/Workspace_company/train_w1/training_w4/task_w4/socket/IpcProtocol.hpp))

* **Đường dẫn Socket**: `SOCKET_PATH = "/tmp/embedded_ipc.sock"`
  * Địa chỉ nhận diện IPC trong Unix Domain Socket là một đường dẫn file thực tế trên Linux Virtual File System (`/tmp`).
* **Cấu trúc `SensorPacket`**:
  ```cpp
  struct SensorPacket {
      int sensorID;
      float temperature;
      float humidity;
      int batteryLevel;
      char statusMsg[64];
      void printInfo();
  };
  ```
  * Cấu trúc gói tin nhị phân định kích thước cố định (`sizeof(SensorPacket)`), giúp quá trình truyền/nhận trực tiếp qua thanh ghi socket mà không cần Serialize/Deserialize phức tạp.

---

### 3.2. Lập Trình Phía Server ([UnixServer.cpp](file:///home/khanh/Workspace_company/train_w1/training_w4/task_w4/socket/UnixServer.cpp))

#### **Vòng đời Socket Server (Server Socket Lifecycle):**
1. **Tạo Socket (`socket`)**:
   ```cpp
   m_serverFd = ::socket(AF_UNIX, SOCK_STREAM, 0);
   ```
   * Khoảng địa chỉ `AF_UNIX` (hoặc `AF_LOCAL`) cho giao tiếp IPC nội bộ.
   * Kiểu `SOCK_STREAM` đảm bảo luồng truyền tin cậy, nối kết luồng theo thứ tự tương tự TCP.
2. **Gán địa chỉ (`bind`)**:
   ```cpp
   sockaddr_un addr{};
   addr.sun_family = AF_UNIX;
   std::strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
   ::bind(m_serverFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
   ```
   * Gán socket với đường dẫn tập tin `/tmp/embedded_ipc.sock`.
3. **Lắng nghe kết nối (`listen`)**:
   ```cpp
   ::listen(m_serverFd, 5);
   ```
   * Chuyển socket sang trạng thái lắng nghe kết nối với hàng chờ tối đa 5 kết nối.
4. **Chấp nhận kết nối (`accept`)**:
   ```cpp
   m_clientFd = ::accept(m_serverFd, nullptr, nullptr);
   ```
   * Chờ Client kết nối và trả về một Socket File Descriptor mới (`m_clientFd`) riêng cho Client đó.
5. **Đọc dữ liệu (`recv`)**:
   ```cpp
   ::recv(m_clientFd, &packet, sizeof(SensorPacket), 0);
   ```
   * Nhận dữ liệu trực tiếp vào biến Struct `packet` và hiển thị ra Console bằng `packet.printInfo()`.
6. **Xử lý Tín hiệu (Signal Handling & Cleanup)**:
   ```cpp
   void signalHandler(int signal) {
       g_server.stop();
       exit(0);
   }
   // Trong main:
   signal(SIGINT, signalHandler);
   ```
   * Khi nhấn `Ctrl+C`, `signalHandler` chủ động đóng socket (`close`) và xóa file node (`::unlink(SOCKET_PATH)`).

---

### 3.3. Lập Trình Phía Client ([UnixClient.cpp](file:///home/khanh/Workspace_company/train_w1/training_w4/task_w4/socket/UnixClient.cpp))

#### **Vòng đời Socket Client (Client Socket Lifecycle):**
1. **Tạo Socket**: `::socket(AF_UNIX, SOCK_STREAM, 0)`
2. **Kết nối tới Server (`connect`)**:
   ```cpp
   ::connect(m_clientFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
   ```
   * Chủ động tìm và kết nối tới file socket node do Server khởi tạo tại `/tmp/embedded_ipc.sock`.
3. **Gửi gói dữ liệu (`send`)**:
   ```cpp
   ::send(m_clientFd, &packet, sizeof(SensorPacket), 0);
   ```
   * Client cập nhật các thông số cảm biến ngẫu nhiên/theo chu kỳ và truyền sang Server mỗi 2 giây (`::sleep(2)`).

---

## 4. Hướng Dẫn Biên Dịch & Thử Nghiệm

### Bước 1: Biên dịch bằng `g++` (C++11)

Mở Terminal tại thư mục `socket`:

```bash
# Biên dịch Server
g++ -std=c++11 UnixServer.cpp -o server_cpp

# Biên dịch Client
g++ -std=c++11 UnixClient.cpp -o client_cpp
```

### Bước 2: Chạy thử nghiệm truyền nhận dữ liệu

1. **Mở Terminal 1 (Khởi chạy Server):**
   ```bash
   ./server_cpp
   ```
   *Màn hình Server hiển thị:*
   ```text
   =========================================================
     UNIX DOMAIN SOCKET SERVER C++ ĐANG LẮNG NGHE...
     Socket File Node: /tmp/embedded_ipc.sock
   =========================================================
   ```

2. **Mở Terminal 2 (Khởi chạy Client):**
   ```bash
   ./client_cpp
   ```
   *Màn hình Client hiển thị:*
   ```text
   Connected to Server........
   [Client C++] Sending packet 1 next to Server...
   [Client C++] Sending packet 2 next to Server...
   ```

3. **Màn hình Server nhận dữ liệu từ Client:**
   ```text
   Client Connected........
   Sensor ID: 101
   Temperature: 29.5
   Humidity: 61
   Battery Level: 99
   Status Message: Packet C++ send 1
   ------------------------
   Sensor ID: 101
   Temperature: 30.5
   Humidity: 62
   Battery Level: 98
   Status Message: Packet C++ send 2
   ------------------------
   ```
