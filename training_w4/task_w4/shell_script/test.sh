#!/bin/bash

TARGET=$1

if [ -z "$TARGET" ]; then
    echo "Lỗi: Bạn chưa truyền tên tiến trình!"
    echo "Cách dùng: ./test.sh <tên_tiến_trình> (Ví dụ: ./test.sh zsh hoặc ./test.sh node)"
    exit 1
fi

    MAX_CPU=80.0
    MAX_MEM=50.0
while true; do
    # Tìm PID của tiến trình (bỏ qua PID ($$) của chính script này)
    PID=$(pgrep -f "$TARGET" | grep -v "^$$$" | head -n 1)

    if [ -z "$PID" ]; then
        TIME=$(date +"%Y-%m-%d %H:%M:%S")
        echo "$TIME - Không tìm thấy tiến trình '$TARGET' đang chạy..."
        sleep 2
        continue
    fi

    # Lấy %CPU và %RAM từ PID tìm được
    CPU=$(LC_ALL=C ps -p "$PID" -o %cpu= | xargs)
    MEM=$(LC_ALL=C ps -p "$PID" -o %mem= | xargs)

    TIME=$(date +"%Y-%m-%d %H:%M:%S")
    echo "$TIME - Tiến trình: $TARGET | PID: $PID | CPU: $CPU% | RAM: $MEM%"


    IS_CPU_HIGH=$(awk -v cpu="$CPU" -v max="$MAX_CPU" 'BEGIN { print (cpu > max) ? 1 : 0 }')
    if [ "$IS_CPU_HIGH" == "1" ]; then
        echo "$TIME - CPU usage ($CPU%) is too high! ($CPU% > $MAX_CPU%)"
    fi
    IS_MEM_HIGH=$(awk -v mem="$MEM" -v max="$MAX_MEM" 'BEGIN { print (mem > max) ? 1 : 0 }')
    if [ "$IS_MEM_HIGH" == "1" ]; then
        echo "$TIME - Memory usage ($MEM%) is too high! ($MEM% > $MAX_MEM%)"
    fi
    
    sleep 2
done
