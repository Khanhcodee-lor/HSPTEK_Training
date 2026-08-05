#include "my_ioctl.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h> // <-- Thêm thư viện ioctl của C
#include <unistd.h>

#define DEVICE_PATH "/dev/char_device"

int main() {
  int fd;
  char write_buf[] = "Hello! This is data test from User Space";
  char read_buf[1024] = {0};
  ssize_t ret;

  int send_val = 12345;
  int recv_val = 0;

  fd = open(DEVICE_PATH, O_RDWR);
  if (fd < 0) {
    perror("Failed to open the device (/dev/char_device)");
    return -1;
  }
  printf("Successfully opened device! FD = %d\n", fd);

  // Write to device
  printf("Writing to device: \"%s\"\n", write_buf);
  ret = write(fd, write_buf, strlen(write_buf));
  if (ret < 0) {
    perror("Failed to write to device");
    close(fd);
    return -1;
  }
  printf("Successfully written %zd bytes.\n", ret);

  // Re-open device to reset file offset for reading
  close(fd);
  fd = open(DEVICE_PATH, O_RDWR);
  if (fd < 0) {
    perror("Failed to reopen the device for reading");
    return -1;
  }

  // Read from device
  ret = read(fd, read_buf, sizeof(read_buf) - 1);
  if (ret < 0) {
    perror("Failed to read from device");
    close(fd);
    return -1;
  }

  printf("Successfully read %zd bytes.\n", ret);
  printf("Read content: \"%s\"\n", read_buf);

  printf("\n--- TEST IOCTL COMMANDS ---\n");

  // Test Ghi gia tri qua IOCTL
  printf("IOCTL: Sending value %d to driver...\n", send_val);
  if (ioctl(fd, WR_VALUE, &send_val) < 0) {
    perror("Failed to send IOCTL WR_VALUE");
  } else {
    printf("Successfully sent IOCTL WR_VALUE!\n");
  }
  // Test Doc gia tri qua IOCTL
  printf("IOCTL: Reading value from driver...\n");
  if (ioctl(fd, RD_VALUE, &recv_val) < 0) {
    perror("Failed to receive IOCTL RD_VALUE");
  } else {
    printf("Successfully read IOCTL RD_VALUE = %d!\n", recv_val);
  }

  close(fd);
  return 0;
}
