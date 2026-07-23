#include <iostream>
#include "led_controller.hpp"
#include <thread>
#include <chrono>

int main() {
    std::cout << "===========================================" << std::endl;
    std::cout << "   Hello World!                            " << std::endl;
    std::cout << "   Embedded Linux ARM - LED Control App    " << std::endl;
    std::cout << "===========================================" << std::endl;

    // Pin 18 (e.g. GPIO18 on Raspberry Pi or ARM Board)
    LEDController led(18);

    if (led.init()) {
        std::cout << "\n--- Testing Manual Control ---" << std::endl;
        led.turnOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        led.turnOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::cout << "\n--- Testing Automatic Blinking ---" << std::endl;
        led.blink(5, 300); // Blink 5 times with 300ms interval
    }

    std::cout << "\nEmbedded Linux ARM App Terminated Successfully." << std::endl;
    return 0;
}
