#!/bin/bash
cmake -B build_arm -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
cmake --build build_arm
qemu-aarch64 -L /usr/aarch64-linux-gnu build_arm/HelloArmApp