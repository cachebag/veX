#pragma once
#include <SFML/Graphics.hpp>

class Platform {
public:
    Platform(float x, float y, float width, float height, sf::Texture& outerTexture, sf::Texture& innerTexture1, sf::Texture& innerTexture2, const sf::Vector2u& windowSize);

    void draw(sf::RenderWindow& window);  // Draws the platform
    sf::FloatRect getBounds() const;      // Get the bounding box of the platform

private:
    sf::RectangleShape outerLayer;    // Outer layer representing the top texture (ground.png)
    sf::RectangleShape innerLayer1;   // First inner layer (ground2.png)
    sf::RectangleShape innerLayer2;   // Second inner layer (ground3.png)

    float perlinNoise(float x, float y) const;  // Perlin noise function for randomness
    void createHole(float gapX, float gapY, float gapWidth, float gapHeight);  // Create hole in the platform
};

