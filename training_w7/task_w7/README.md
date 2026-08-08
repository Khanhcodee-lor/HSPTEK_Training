# Báo cáo Tuần 7: Bringup & Device Tree

## 1. Kiến thức đã học (Lý thuyết)
Trong tuần 7, các nội dung lý thuyết trọng tâm đã được tìm hiểu bao gồm:
- **Quá trình Boot (Boot Process)**: Nắm vững luồng khởi động của hệ thống nhúng Linux theo thứ tự: `Bootloader -> Kernel -> Init`.
- **Cú pháp Device Tree (DTS/DTSI)**: Cách mô tả phần cứng không thể tự phát hiện (non-discoverable hardware) thành cấu trúc cây thay vì code trực tiếp vào C.
- **Binding Properties**: Các thuộc tính tiêu chuẩn trong Device Tree để kernel nhận diện cấu hình phần cứng.
- **Platform Driver & Device Matching**: Cơ chế kernel kết nối giữa mô tả phần cứng (Device Tree) với driver tương ứng thông qua thuộc tính `compatible`.
- **Các Framework cơ bản trong Kernel**:
  - **Pinctrl**: Quản lý và cấu hình chức năng của các chân tín hiệu (multiplexing/pin configuration).
  - **Clock Framework**: Quản lý hệ thống xung nhịp, phân phối xung nhịp đến các ngoại vi.
  - **Regulator Framework**: Quản lý cấu hình nguồn điện cung cấp cho các module thiết bị.

---

## 2. Áp dụng Thực hành
Phần thực hành tập trung vào việc đọc, sửa DTS cho board mạch thực tế và tiến hành thêm node thiết bị ảo/thật, sau đó Boot kernel để kiểm tra.

### 2.1. Cấu trúc thư mục thực hành
```text
task_w7/
├── makefile                # Kịch bản biên dịch DTS sang DTBO, clean và install
├── overlays/               # Thư mục chứa mã nguồn Device Tree Overlays
│   ├── dummy_device.dts    # File mô phỏng thiết bị nguồn và xung nhịp ảo
│   └── my_led.dts          # File cấu hình chân GPIO điều khiển LED thực tế
└── build/                  # Thư mục chứa file nhị phân (.dtbo)
```

### 2.2. Chi tiết cấu hình Device Tree (DTS)

#### A. Thiết bị điều khiển LED (`my_led.dts`)
- **Pinctrl**: Cấu hình chân GPIO 21 của chip BCM2711 làm chân output không có điện trở kéo nội (`pull = <0>`).
- **Device Node**: Thêm một thiết bị LED, sử dụng driver có sẵn `gpio-leds` (`compatible = "gpio-leds"`). Cấu hình cho LED nhấp nháy theo nhịp hệ thống thông qua `linux,default-trigger = "heartbeat"`.

#### B. Thiết bị giả lập Clock và Regulator (`dummy_device.dts`)
- **Dummy Clock (`dummy_clk_50mhz`)**: Mô phỏng một bộ tạo dao động ảo tần số **50MHz** bằng cách sử dụng `compatible = "fixed-clock"` và đặt thuộc tính `clock-frequency = <50000000>`.
- **Dummy Regulator (`dummy_vreg_3v3`)**: Mô phỏng IC cấp nguồn ảo có đầu ra ổn định ở mức **3.3V**, sử dụng `compatible = "regulator-fixed"` và thêm thuộc tính `regulator-always-on` để đảm bảo nguồn này luôn được kích hoạt.

---

## 3. Các bước triển khai & Kiểm tra

### Bước 1: Biên dịch file DTS sang DTBO
Mở terminal trong thư mục `task_w7` và chạy lệnh:
```bash
make
```
Makefile sẽ gọi công cụ `dtc` để biên dịch các file `.dts` thành chuẩn nhị phân `.dtbo` và đưa vào thư mục `build/`.

### Bước 2: Nạp Device Tree Overlays (DTBO)
Chạy lệnh cài đặt với quyền quản trị viên:
```bash
sudo make install
```
Lệnh này tự động chép các file `.dtbo` vào phân vùng boot của board (`/boot/firmware/overlays/`). Tiếp theo, khai báo trong file `/boot/firmware/config.txt` để Bootloader nạp chúng lúc khởi động bằng cách thêm:
```ini
dtoverlay=my_led
dtoverlay=dummy_device
```

### Bước 3: Boot Kernel và Kiểm tra
Khởi động lại board mạch:
```bash
sudo reboot
```

Kiểm tra kernel đã nhận dạng thành công Device Tree mới và đưa vào cây thông tin trong hệ thống ảo `/proc/device-tree` chưa:
```bash
# 1. Kiểm tra thiết bị thật (LED)
ls -l /proc/device-tree/my_custom_leds/

# 2. Kiểm tra các node giả lập vừa thêm
ls -l /proc/device-tree/dummy_clk_50mhz/
ls -l /proc/device-tree/dummy_vreg_3v3/
```
