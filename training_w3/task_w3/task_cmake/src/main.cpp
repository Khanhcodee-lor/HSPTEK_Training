#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "geometry/shape_factory.hpp"
#include "utils/logger.hpp"

int main() {
    Utils::log("APP", "Starting Advanced Geometry CMake Application...");

    std::vector<std::shared_ptr<Shape>> shapes;
    shapes.push_back(ShapeFactory::createCircle(5.0));
    shapes.push_back(ShapeFactory::createRectangle(4.0, 6.0));
    shapes.push_back(ShapeFactory::createTriangle(3.0, 4.0, 5.0));
    shapes.push_back(ShapeFactory::createCircle(2.5));

    // Sort shapes by Area descending
    std::sort(shapes.begin(), shapes.end(), [](const auto& s1, const auto& s2) {
        return s1->area() > s2->area();
    });

    Utils::log("APP", "Shapes sorted by Area (Descending):");
    for (const auto& shape : shapes) {
        std::string info = shape->name() + " -> Area: " + std::to_string(shape->area()) 
                         + ", Perimeter: " + std::to_string(shape->perimeter());
        Utils::log("SHAPE", info);
    }

    return 0;
}
