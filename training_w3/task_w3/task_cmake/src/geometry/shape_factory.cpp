#include "geometry/shape_factory.hpp"
#include "geometry/circle.hpp"
#include "geometry/rectangle.hpp"
#include "geometry/triangle.hpp"

std::shared_ptr<Shape> ShapeFactory::createCircle(double radius) {
    return std::make_shared<Circle>(radius);
}

std::shared_ptr<Shape> ShapeFactory::createRectangle(double width, double height) {
    return std::make_shared<Rectangle>(width, height);
}

std::shared_ptr<Shape> ShapeFactory::createTriangle(double a, double b, double c) {
    return std::make_shared<Triangle>(a, b, c);
}
