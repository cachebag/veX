// Platform.cpp
#include "../include/Platform.hpp"

Platform::Platform(float x, float y, float width, float height, sf::Color color) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(x, y);
    shape.setFillColor(color);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}

void Platform::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

