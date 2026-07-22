#include "geometry/triangle.hpp"
#include <cmath>

Triangle::Triangle(double side_a, double side_b, double side_c) 
    : a(side_a), b(side_b), c(side_c) {}

double Triangle::area() const {
    double s = (a + b + c) / 2.0;
    return std::sqrt(s * (s - a) * (s - b) * (s - c));
}

double Triangle::perimeter() const {
    return a + b + c;
}

std::string Triangle::name() const {
    return "Triangle (a=" + std::to_string(a) + ", b=" + std::to_string(b) + ", c=" + std::to_string(c) + ")";
}
