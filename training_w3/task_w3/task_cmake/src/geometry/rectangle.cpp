#include "geometry/rectangle.hpp"

Rectangle::Rectangle(double w, double h) : width(w), height(h) {}

double Rectangle::area() const {
    return width * height;
}

double Rectangle::perimeter() const {
    return 2 * (width + height);
}

std::string Rectangle::name() const {
    return "Rectangle (w=" + std::to_string(width) + ", h=" + std::to_string(height) + ")";
}
