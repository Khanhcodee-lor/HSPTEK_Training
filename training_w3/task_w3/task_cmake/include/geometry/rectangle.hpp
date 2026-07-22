#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "geometry/shape.hpp"

class Rectangle : public Shape {
private:
    double width;
    double height;
public:
    Rectangle(double w, double h);
    double area() const override;
    double perimeter() const override;
    std::string name() const override;
};

#endif // RECTANGLE_HPP
