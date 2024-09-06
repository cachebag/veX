// Platform.hpp
#pragma once
#include <SFML/Graphics.hpp>

class Platform {
public:
    // Constructor that accepts window size for scaling
    Platform(float x, float y, float width, float height, sf::Color color, const sf::Vector2u& windowSize);

    sf::FloatRect getBounds() const; // Get the bounding box of the platform
    void draw(sf::RenderWindow& window) const; // Draw the platform

private:
    sf::RectangleShape shape; // The graphical representation of the platform
};

