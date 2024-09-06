// Platform.cpp
#include "../include/Platform.hpp"

Platform::Platform(float x, float y, float width, float height, sf::Color color, const sf::Vector2u& windowSize) {
    // Scale platform dimensions and position based on window size
    shape.setSize(sf::Vector2f(width * windowSize.x, height * windowSize.y)); // width and height as percentages
    shape.setPosition(x * windowSize.x, y * windowSize.y);  // x and y as percentages
    shape.setFillColor(color);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}

void Platform::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

