#include "geometry/circle.hpp"
#include <cmath>

Circle::Circle(double r) : radius(r) {}

double Circle::area() const {
    return M_PI * radius * radius;
}

double Circle::perimeter() const {
    return 2 * M_PI * radius;
}

std::string Circle::name() const {
    return "Circle (r=" + std::to_string(radius) + ")";
}
