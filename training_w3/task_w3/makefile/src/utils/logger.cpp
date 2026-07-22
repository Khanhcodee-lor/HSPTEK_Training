#include "utils/logger.hpp"
#include <iostream>

void log_info(const std::string& message) {
    std::cout << "[INFO]: " << message << std::endl;
}

void log_debug(const std::string& message) {
#ifdef DEBUG
    std::cout << "[DEBUG]: " << message << std::endl;
#else
    (void)message;
#endif
}
