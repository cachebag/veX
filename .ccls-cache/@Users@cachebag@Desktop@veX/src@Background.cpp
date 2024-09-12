#include "../include/Background.hpp"
#include <iostream>

Background::Background(const std::string& filePath) {

    if (!backgroundTexture.loadFromFile(filePath)) {
        // Handle error loading texture
        std::cerr << "Error loading background texture from " << filePath << std::endl;
    }

    backgroundSprite.setTexture(backgroundTexture);
}

void Background::render(sf::RenderWindow& window, const sf::Vector2u& windowSize) {

    sf::Vector2u textureSize = backgroundTexture.getSize();

    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

    backgroundSprite.setScale(scaleX, scaleY);
    window.draw(backgroundSprite);
}

