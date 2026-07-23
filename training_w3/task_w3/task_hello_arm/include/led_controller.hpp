#ifndef LED_CONTROLLER_HPP
#define LED_CONTROLLER_HPP

#include <string>

class LEDController {
private:
    int gpio_pin;
    bool is_on;
    std::string gpio_path;

public:
    explicit LEDController(int pin);
    ~LEDController();

    bool init();
    void turnOn();
    void turnOff();
    void toggle();
    void blink(int count, int delay_ms);
    bool getState() const;
};

#endif // LED_CONTROLLER_HPP
