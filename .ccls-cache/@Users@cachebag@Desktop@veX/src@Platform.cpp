#include "../include/Platform.hpp"
#include <cmath>

// Constructor initializes the platform and sets up layers
Platform::Platform(float x, float y, float width, float height, sf::Texture& outerTexture, sf::Texture& innerTexture1, sf::Texture& innerTexture2, const sf::Vector2u& windowSize) {
    // Set platform size based on window dimensions
    sf::Vector2f platformSize(width * windowSize.x, height * windowSize.y);

    // Outer layer (ground.png)
    outerLayer.setSize(platformSize);
    outerLayer.setPosition(x * windowSize.x, y * windowSize.y);
    outerTexture.setRepeated(true);  // Enable texture repetition for outer layer
    outerLayer.setTexture(&outerTexture);

    // Set the texture rectangle size based on platform size (repeating the texture)
    outerLayer.setTextureRect(sf::IntRect(0, 0, static_cast<int>(outerLayer.getSize().x), static_cast<int>(outerLayer.getSize().y)));

    // Inner layer 1 (ground2.png)
    innerLayer1.setSize(platformSize);
    innerLayer1.setPosition(x * windowSize.x, y * windowSize.y);
    innerTexture1.setRepeated(true);  // Enable repetition for inner layer 1
    innerLayer1.setTexture(&innerTexture1);
    innerLayer1.setTextureRect(sf::IntRect(0, 0, static_cast<int>(innerLayer1.getSize().x), static_cast<int>(innerLayer1.getSize().y)));

    // Inner layer 2 (ground3.png)
    innerLayer2.setSize(platformSize);
    innerLayer2.setPosition(x * windowSize.x, y * windowSize.y);
    innerTexture2.setRepeated(true);  // Enable repetition for inner layer 2
    innerLayer2.setTexture(&innerTexture2);
    innerLayer2.setTextureRect(sf::IntRect(0, 0, static_cast<int>(innerLayer2.getSize().x), static_cast<int>(innerLayer2.getSize().y)));

    // Create a hole in the platform
    createHole(0.4f, 0.5f, 0.2f, 0.05f);  // Adjust gap size and position as needed
}


// Create the hole by cutting out part of the platform
void Platform::createHole(float gapX, float gapY, float gapWidth, float gapHeight) {
    sf::FloatRect hole(gapX * outerLayer.getSize().x, gapY * outerLayer.getSize().y, gapWidth * outerLayer.getSize().x, gapHeight * outerLayer.getSize().y);
    
    // You can handle the hole rendering logic here, using shaders or masking if necessary.
    // This is a placeholder to visualize where the hole would be.
    // Add logic to modify vertices or shapes to represent the hole visually if required.
}

// Perlin Noise function for randomness
float Platform::perlinNoise(float x, float y) const {
    return static_cast<float>(std::sin(2.0 * M_PI * x) * std::sin(2.0 * M_PI * y));  // Simplified Perlin noise
}

// Get the bounding box of the platform
sf::FloatRect Platform::getBounds() const {
    return outerLayer.getGlobalBounds();
}

// Draw the platform, applying Perlin noise and handling multiple layers
void Platform::draw(sf::RenderWindow& window) {
    // Apply Perlin noise effect to the outer layer for randomness
    sf::RectangleShape tempOuterLayer = outerLayer;
    tempOuterLayer.setScale(1.0f + perlinNoise(outerLayer.getPosition().x, outerLayer.getPosition().y) * 0.1f,
                            1.0f + perlinNoise(outerLayer.getPosition().x, outerLayer.getPosition().y) * 0.1f);

    // Draw the deeper layers first, then the outermost layer
    window.draw(innerLayer2);    // Draw the deepest layer (ground3)
    window.draw(innerLayer1);    // Draw the middle layer (ground2)
    window.draw(tempOuterLayer); // Draw the modified outer layer (ground.png)
}

