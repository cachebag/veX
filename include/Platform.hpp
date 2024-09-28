#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Platform {
public:
    Platform(float x, float y, float width, float height, const sf::Texture& texture);

    void draw(sf::RenderWindow& window);  // Draws the tiled platform
    sf::FloatRect getBounds() const;      // Get the bounding box of the platform

    const std::vector<sf::RectangleShape>& getTiles() const;  // Get individual tiles for collision detection

private:
    std::vector<sf::RectangleShape> tiles;  // A collection of tiled rectangles
};

