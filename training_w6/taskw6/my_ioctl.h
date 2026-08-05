#ifndef MY_IOCTL_H
#define MY_IOCTL_H

#include <linux/ioctl.h>

#define WR_MAGIC 'a'

// Định nghĩa mã lệnh: GHI một số nguyên xuống Driver
#define WR_VALUE _IOW(WR_MAGIC, 1, int*)

// Định nghĩa mã lệnh: ĐỌC một số nguyên từ Driver về User
#define RD_VALUE _IOR(WR_MAGIC, 2, int*)

#endif