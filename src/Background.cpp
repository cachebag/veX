#include "../include/Background.hpp"
#include <iostream>

Background::Background(const std::string& backgroundFilePath, const std::string& middlegroundFilePath, const sf::Vector2u& windowSize) {

    if (!backgroundTexture.loadFromFile(backgroundFilePath)) {
        // Handle error loading texture
        std::cerr << "Error loading background texture from " << backgroundFilePath << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);

    if (!middlegroundTexture.loadFromFile(middlegroundFilePath)) {
        std::cerr << "Error loading middleground texture from " << middlegroundFilePath << std::endl;
    }
    middlegroundTexture.setRepeated(true);

    middlegroundSprite.setTextureRect(sf::IntRect(0, 0, windowSize.x, middlegroundTexture.getSize().y));

    middlegroundSprite.setTexture(middlegroundTexture);
}

void Background::render(sf::RenderWindow& window, const sf::Vector2u& windowSize) {
    // Render the background (same as before)
    sf::Vector2u textureSize = backgroundTexture.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    backgroundSprite.setScale(scaleX, scaleY);
    window.draw(backgroundSprite);

    // Calculate scale for middleground while maintaining aspect ratio
    sf::Vector2u middleTextureSize = middlegroundTexture.getSize();

    // Calculate aspect ratios
    float aspectRatio = static_cast<float>(middleTextureSize.x) / middleTextureSize.y;
    float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;

    // Scale middleground to fit horizontally and preserve aspect ratio
    float middleScaleX, middleScaleY;
    if (windowAspectRatio > aspectRatio) {
        // If the window is wider than the texture's aspect ratio, scale based on height
        middleScaleY = static_cast<float>(windowSize.y) / middleTextureSize.y;
        middleScaleX = middleScaleY; // Preserve aspect ratio
    } else {
        // If the window is taller than the texture's aspect ratio, scale based on width
        middleScaleX = static_cast<float>(windowSize.x) / middleTextureSize.x;
        middleScaleY = middleScaleX; // Preserve aspect ratio
    }

    middlegroundSprite.setScale(middleScaleX, middleScaleY);

    // Position the middleground (center it vertically or horizontally if necessary)
    float middlePositionX = (windowSize.x - middleTextureSize.x * middleScaleX) / 2.0f;
    float middlePositionY = (windowSize.y - middleTextureSize.y * middleScaleY) / 2.0f;
    middlegroundSprite.setPosition(0.0f, middlePositionY);

    window.draw(middlegroundSprite);

}

