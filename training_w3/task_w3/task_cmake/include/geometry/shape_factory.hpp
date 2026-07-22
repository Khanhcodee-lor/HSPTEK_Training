#ifndef SHAPE_FACTORY_HPP
#define SHAPE_FACTORY_HPP

#include "geometry/shape.hpp"
#include <memory>

class ShapeFactory {
public:
    static std::shared_ptr<Shape> createCircle(double radius);
    static std::shared_ptr<Shape> createRectangle(double width, double height);
    static std::shared_ptr<Shape> createTriangle(double a, double b, double c);
};

#endif // SHAPE_FACTORY_HPP
