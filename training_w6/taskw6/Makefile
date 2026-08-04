# Chỉ định tên file .o tương ứng với Kernel Module (main.c)
obj-m += main.o
export QEMU_LD_PREFIX := /usr/arm-linux-gnueabihf

# Khai bao các biến cho Biên dịch chéo (ARM 32-bit)
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

# Đường dẫn Kernel Build, Thư mục hiện tại và Thư mục Build
KDIR    := $(HOME)/bbb_headers/usr/src/linux-headers-5.10.168-ti-r72
PWD     := $(shell pwd)
OUT_DIR := $(PWD)/build

# Target mặc định: Build Kernel Module & User Space App, sau đó chuyển toàn bộ vào build/
all: module user_test
	@mkdir -p $(OUT_DIR)
	@mv -f *.ko *.o *.mod *.mod.c *.order *.symvers .*.cmd $(OUT_DIR)/ 2>/dev/null || true
	@if [ -f user_test ]; then mv -f user_test $(OUT_DIR)/; fi
	@echo "Cross-compilation successful! All build artifacts moved to $(OUT_DIR)/"

module:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

user_test: user_test.c
	$(CROSS_COMPILE)gcc -Wall -O2 -static user_test.c -o user_test
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean
	rm -rf $(OUT_DIR)

.PHONY: all module user_test clean