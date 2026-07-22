#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "geometry/shape.hpp"

class Triangle : public Shape {
private:
    double a, b, c;
public:
    Triangle(double side_a, double side_b, double side_c);
    double area() const override;
    double perimeter() const override;
    std::string name() const override;
};

#endif // TRIANGLE_HPP
