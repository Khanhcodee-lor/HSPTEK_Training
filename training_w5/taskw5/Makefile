obj-m += hello.o

KDIR ?= $(HOME)/android-kernel/common
CLANG_BIN ?= $(HOME)/android-kernel/prebuilts/clang/host/linux-x86/clang-r487747c/bin
OUT_DIR ?= $(PWD)/build

all:
	PATH=$(CLANG_BIN):$(PATH) make -C $(KDIR) M=$(PWD) ARCH=x86_64 LLVM=1 modules
	@mkdir -p $(OUT_DIR)
	@mv -f *.ko *.o *.mod *.mod.c *.order *.symvers .*.cmd $(OUT_DIR)/ 2>/dev/null || true

clean:
	PATH=$(CLANG_BIN):$(PATH) make -C $(KDIR) M=$(PWD) ARCH=x86_64 LLVM=1 clean
	rm -rf $(OUT_DIR)