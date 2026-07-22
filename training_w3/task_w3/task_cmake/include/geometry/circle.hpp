#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "geometry/shape.hpp"

class Circle : public Shape {
private:
    double radius;
public:
    explicit Circle(double r);
    double area() const override;
    double perimeter() const override;
    std::string name() const override;
};

#endif // CIRCLE_HPP
