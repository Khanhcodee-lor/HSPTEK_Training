# Trình biên dịch Device Tree và các cờ
DTC = dtc
DTC_FLAGS = -@ -I dts -O dtb

# Khai báo các thư mục
SRC_DIR = ./overlays
BUILD_DIR = ./build
OUT_DIR = /boot/firmware/overlays

# Tự động tìm các file .dts trong thư mục overlays
SRCS = $(wildcard $(SRC_DIR)/*.dts)

# Tạo danh sách file .dtbo tương ứng trong build/
OBJS = $(patsubst $(SRC_DIR)/%.dts, $(BUILD_DIR)/%.dtbo, $(SRCS))

# Mục tiêu mặc định
all: $(OBJS)

# Lệnh biên dịch từng file .dts sang .dtbo
$(BUILD_DIR)/%.dtbo: $(SRC_DIR)/%.dts | $(BUILD_DIR)
	@echo "[DTC] Biên dịch $< -> $@"
	$(DTC) $(DTC_FLAGS) -o $@ $<

# Tự động tạo thư mục build nếu chưa tồn tại
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Lệnh nạp trực tiếp vào Raspberry Pi
install: all
	@echo "[INSTALL] Cài đặt các file .dtbo vào $(OUT_DIR)"
	sudo cp $(BUILD_DIR)/*.dtbo $(OUT_DIR)/
	@echo "Hoàn tất! Hãy kiểm tra lại dòng dtoverlay=... trong file config.txt"

# Lệnh dọn dẹp thư mục build
clean:
	@echo "[CLEAN] Dọn dẹp thư mục $(BUILD_DIR)"
	rm -rf $(BUILD_DIR)/*.dtbo

.PHONY: all clean install
