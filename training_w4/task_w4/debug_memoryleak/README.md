# Báo Cáo Task: Debug Memory Leak & Quản Lý Bộ Nhớ Động Trong C

## 1. Tổng Quan & Mục Tiêu

Báo cáo này mô tả chi tiết quá trình phát hiện lỗi, sửa lỗi biên dịch, xử lý rò rỉ bộ nhớ (Memory Leak) và tối ưu hóa quản lý bộ nhớ động cho dự án **Device Manager App** thuộc học phần *Embedded Linux cơ bản*.

### Mục tiêu chính:
- **Khắc phục lỗi biên dịch & xung đột prototype**: Sửa các lỗi cú pháp, thiếu dấu chấm phẩy, khai báo thiếu hàm và không đồng bộ giữa file Header (`.h`) và Source (`.c`).
- **Sửa lỗi Logic & Crash chương trình**: Khắc phục lỗi dùng sai toán tử so sánh `==` thay vì phép gán `=` khi tái cấp phát bộ nhớ (`realloc`).
- **Xử lý triệt để Memory Leak (Deep Freeing)**: Thiết kế cơ chế giải phóng bộ nhớ đa tầng cho các Struct phức tạp có chứa con trỏ động lồng nhau (`Device` -> `deviceName`, `ipAddress`, `sensorList` -> `sensorName`).
- **Xác minh sạch rò rỉ bộ nhớ bằng Valgrind**: Đảm bảo chương trình giải phóng 100% bộ nhớ được cấp phát trên Heap (`All heap blocks were freed -- no leaks are possible`).

---

## 2. Cấu Trúc Dự Án

```text
debug_memoryleak/
├── CMakeLists.txt      # File cấu hình biên dịch CMake (bật cờ Debug -g -O0)
├── README.md           # Báo cáo chi tiết task
├── app/
│   └── main.c          # Chương trình chính (điểm chạy ứng dụng)
├── include/
│   ├── device.h        # Header khai báo Struct Device và các hàm quản lý Device
│   └── sensor.h        # Header khai báo Struct Sensor và các hàm quản lý Sensor
└── src/
    ├── device.c        # Định nghĩa các hàm cấp phát, danh sách liên kết và giải phóng Device
    └── sensor.c        # Định nghĩa các hàm tạo và giải phóng bộ nhớ của Sensor
```

---

## 3. Chi Tiết Lỗi Phát Hiện & Giải Pháp Khắc Phục (Bug Fixes)

### 3.1. Phân Tích Lỗi Biên Dịch & Khai Báo (Compilation Errors)

1. **File `app/main.c`**:
   - *Lỗi*: Thiếu dấu chấm phẩy `;` ở dòng khởi tạo `Device* head = NULL`.
   - *Khắc phục*: Thêm `;` vào cuối dòng.
   - *Lỗi*: Gọi hàm `printDeviceList(head);` nhưng hàm chưa được khai báo trong [include/device.h](file:///home/khanh/Workspace_company/train_w1/training_w4/task_w4/debug_memoryleak/include/device.h).
   - *Khắc phục*: Khai báo `void printDeviceList(Device* head);` trong [include/device.h](file:///home/khanh/Workspace_company/train_w1/training_w4/task_w4/debug_memoryleak/include/device.h#L19).

2. **File `include/device.h` vs `src/device.c`**:
   - *Lỗi*: Trong `device.h`, khai báo `Device createDevice(...)` trả về kiểu Struct value, nhưng trong `device.c` lại định nghĩa `Device* createDevice(...)` trả về con trỏ `Device*`.
   - *Khắc phục*: Đồng bộ kiểu trả về của `createDevice` thành `Device*` ở cả file Header và Source.

3. **File `include/sensor.h` vs `src/sensor.c`**:
   - *Lỗi 1*: Không đồng bộ kiểu của tham số tên cảm biến (`char*` vs `const char*`). Đã đồng bộ về `const char* sensorName`.
   - *Lỗi 2*: Trong `src/sensor.c`, hàm `freeSensorContent` sử dụng tên biến `s` và thuộc tính `s->sensor_name` (không tồn tại trong Struct `Sensor`).
   - *Khắc phục*: Đổi tên biến thành `sensor` và thuộc tính thành `sensor->sensorName`.

---

### 3.2. Lỗi Logic Dẫn Đến Crash (Segmentation Fault)

- **Tại `src/device.c` dòng 27-28**:
  ```c
  // Lỗi ban đầu:
  device->sensorList == (Sensor*) realloc(device->sensorList, device->sensorCount * sizeof(Sensor));
  ```
  - *Nguyên nhân*: Sử dụng nhầm phép so sánh `==` thay vì phép gán `=`. Dẫn đến `device->sensorList` không bao giờ được cập nhật (vẫn giữ giá trị `NULL`). Khi gán dữ liệu vào `device->sensorList[idx]` sẽ làm ứng dụng bị **Segmentation Fault**.
  - *Khắc phục*: Đổi về phép gán:
    ```c
    device->sensorList = (Sensor *)realloc(device->sensorList, device->sensorCount * sizeof(Sensor));
    ```

---

### 3.3. Lỗi Rò Rỉ Bộ Nhớ (Memory Leak) & Giải Pháp Deep Free

#### Vấn đề ban đầu:
Khi xóa một node `Device` bằng `removeDeviceById` hoặc xóa toàn bộ danh sách bằng `freeDevice`, code cũ chỉ gọi `free(current)` hoặc giải phóng không đầy đủ. Do Struct `Device` chứa các con trỏ cấp phát động lồng nhau, điều này khiến toàn bộ chuỗi ký tự (`deviceName`, `ipAddress`) và mảng cảm biến (`sensorList` + `sensorName`) bị rò rỉ bộ nhớ (leaked memory).

#### Giải pháp (Xây dựng luồng Deep Freeing):

1. **Giải phóng Sensor nội hàm (`freeSensorContent`)**:
   ```c
   void freeSensorContent(Sensor *sensor) {
     if (sensor && sensor->sensorName) {
       free(sensor->sensorName);
       sensor->sensorName = NULL;
     }
   }
   ```

2. **Xây dựng hàm Helper `freeDeviceContent`**:
   Hàm này chuyên trách giải phóng toàn bộ các tài nguyên động được nạp bên trong một `Device`:
   ```c
   void freeDeviceContent(Device *device) {
     if (device == NULL) return;
     if (device->deviceName) free(device->deviceName);
     if (device->ipAddress) free(device->ipAddress);
     if (device->sensorList) {
       for (int i = 0; i < device->sensorCount; i++) {
         freeSensorContent(&(device->sensorList[i]));
       }
       free(device->sensorList);
     }
   }
   ```

3. **Áp dụng giải phóng triệt để khi xóa Node (`removeDeviceById` & `freeDevice`)**:
   - Khi xóa một thiết bị bất kỳ trong danh sách liên kết đơn, gọi `freeDeviceContent(current)` trước khi `free(current)`.
   - Khi giải phóng toàn bộ danh sách (`freeDevice`), duyệt qua từng node, gọi `freeDeviceContent(current)` rồi mới `free(current)`.

---

## 4. Kiến Thức Kỹ Thuật Áp Dụng

1. **Dynamic Memory Allocation (Cấp phát bộ nhớ động)**:
   - `malloc`: Cấp phát vùng nhớ mới trên Heap cho Struct và chuỗi ký tự (`strlen(str) + 1`).
   - `realloc`: Tái cấp phát và mở rộng mảng động `sensorList` khi thêm cảm biến mới.
   - `free`: Giải phóng bộ nhớ trên Heap sau khi hoàn tất sử dụng để tránh cạn kệt tài nguyên.
2. **Data Structure (Danh sách liên kết đơn - Singly Linked List)**:
   - Thêm node vào cuối danh sách (`addDeviceToList`).
   - Tìm kiếm và xóa node theo ID (`removeDeviceById`) duy trì tính liên tục của con trỏ `headRef`.
3. **Kỹ thuật Deep Freeing (Giải phóng đa tầng)**:
   - Áp dụng nguyên tắc: **Cấp phát ở đâu / Cấp phát bao nhiêu tầng -> Giải phóng đúng thứ tự ngược lại bấy nhiêu tầng**.
4. **Kiểm thử bộ nhớ với Valgrind**:
   - Phân tích log báo cáo từ Valgrind (`definitely lost`, `indirectly lost`, `possibly lost`).

---

## 5. Hướng Dẫn Biên Dịch & Kiểm Trực Tiếp Với Valgrind

### 5.1. Các bước biên dịch với CMake

 Mở Terminal tại thư mục `debug_memoryleak`:

```bash
# 1. Tạo thư mục build và di chuyển vào
mkdir -p build && cd build

# 2. Tạo Makefile bằng CMake
cmake ..

# 3. Biên dịch chương trình
make
```

### 5.2. Chạy ứng dụng và kiểm tra Valgrind

1. **Chạy ứng dụng thông thường:**
   ```bash
   ./device_app
   ```

2. **Kiểm tra rò rỉ bộ nhớ với Valgrind:**
   ```bash
   valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./device_app
   ```
