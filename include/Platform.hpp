#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Platform {
public:
    // Constructor now takes an additional isGrassy flag to handle grassy platforms
    Platform(float x, float y, float width, float height, const sf::Texture& texture, bool isGrassy = false);

    void draw(sf::RenderWindow& window);  // Draws the tiled platform
    sf::FloatRect getBounds() const;      // Get the bounding box of the platform

    const std::vector<sf::RectangleShape>& getTiles() const;  // Get individual tiles for collision detection
    void rescale(float scaleX, float scaleY);  // Rescale platform tiles when window is resized

    bool checkCollision(const sf::FloatRect& playerBounds) const;  // Check if the player collides with the platform

private:
    std::vector<sf::RectangleShape> tiles;  // A collection of tiled rectangles
    sf::FloatRect collisionBounds;  // Stores the platform's collision bounds
};

