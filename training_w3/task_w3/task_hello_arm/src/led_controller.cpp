#include "led_controller.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

LEDController::LEDController(int pin) : gpio_pin(pin), is_on(false) {
    gpio_path = "/sys/class/gpio/gpio" + std::to_string(pin);
}

LEDController::~LEDController() {
    turnOff();
}

bool LEDController::init() {
    std::cout << "[ARM LED Driver] Initializing GPIO Pin " << gpio_pin << "..." << std::endl;
    
    // Try Sysfs GPIO export (standard Linux Sysfs interface for ARM boards)
    std::ofstream export_file("/sys/class/gpio/export");
    if (export_file.is_open()) {
        export_file << gpio_pin;
        export_file.close();
        
        std::ofstream dir_file(gpio_path + "/direction");
        if (dir_file.is_open()) {
            dir_file << "out";
            dir_file.close();
        }
        std::cout << "[ARM LED Driver] Hardware GPIO " << gpio_pin << " exported successfully!" << std::endl;
    } else {
        std::cout << "[ARM LED Driver] Running in Simulation Mode (No physical Sysfs GPIO detected)." << std::endl;
    }
    return true;
}

void LEDController::turnOn() {
    is_on = true;
    std::ofstream val_file(gpio_path + "/value");
    if (val_file.is_open()) {
        val_file << "1";
        val_file.close();
    }
    std::cout << "[LED Pin " << gpio_pin << "] -> HIGH (💡 LED ON)" << std::endl;
}

void LEDController::turnOff() {
    is_on = false;
    std::ofstream val_file(gpio_path + "/value");
    if (val_file.is_open()) {
        val_file << "0";
        val_file.close();
    }
    std::cout << "[LED Pin " << gpio_pin << "] -> LOW  (⬛ LED OFF)" << std::endl;
}

void LEDController::toggle() {
    if (is_on) {
        turnOff();
    } else {
        turnOn();
    }
}

void LEDController::blink(int count, int delay_ms) {
    std::cout << "[ARM LED Driver] Starting Blink Sequence: " << count << " times, delay " << delay_ms << "ms" << std::endl;
    for (int i = 0; i < count; ++i) {
        turnOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        turnOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    std::cout << "[ARM LED Driver] Blink Sequence Completed!" << std::endl;
}

bool LEDController::getState() const {
    return is_on;
}
