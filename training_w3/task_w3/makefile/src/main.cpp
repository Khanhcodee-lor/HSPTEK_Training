#include <iostream>
#include "core/calculator.hpp"
#include "utils/logger.hpp"

int main() {
    log_info("Starting Advanced Makefile Application...");
    log_debug("Debug mode is ACTIVE!");

    Calculator calc;
    int sum = calc.add(15, 27);
    int prod = calc.multiply(6, 7);

    log_info("15 + 27 = " + std::to_string(sum));
    log_info("6 * 7 = " + std::to_string(prod));

    return 0;
}
//