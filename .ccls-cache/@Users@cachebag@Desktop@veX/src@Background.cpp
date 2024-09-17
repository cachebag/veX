#include "../include/Background.hpp"
#include <iostream>
#include <cmath>

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

void Background::render(sf::RenderWindow& window, const sf::Vector2u& windowSize, float playerX, float deltaTime) {
    // --- RENDER BACKGROUND WITH PARALLAX EFFECT AND SCALING ---

    // Set the background texture to repeat horizontally
    backgroundTexture.setRepeated(true);

    // Parallax factor for background
    float backgroundParallaxFactor = 0.1f;

    // Calculate background offset
    float backgroundOffsetX = playerX * backgroundParallaxFactor;

    // Wrap the background offset to prevent texture coordinate overflow
    float backgroundTextureWidth = static_cast<float>(backgroundTexture.getSize().x);
    backgroundOffsetX = fmod(backgroundOffsetX, backgroundTextureWidth);
    if (backgroundOffsetX < 0) backgroundOffsetX += backgroundTextureWidth;

    // Calculate scaling factors to fit the window size, maintaining aspect ratio
    float backgroundTextureHeight = static_cast<float>(backgroundTexture.getSize().y);
    float backgroundAspectRatio = backgroundTextureWidth / backgroundTextureHeight;
    float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;

    float backgroundScale;
    if (windowAspectRatio > backgroundAspectRatio) {
        // Window is wider than texture aspect ratio
        backgroundScale = static_cast<float>(windowSize.y) / backgroundTextureHeight;
    } else {
        // Window is narrower than texture aspect ratio
        backgroundScale = static_cast<float>(windowSize.x) / backgroundTextureWidth;
    }

    // Adjusted texture dimensions after scaling
    float scaledTextureWidth = backgroundTextureWidth * backgroundScale;
    float scaledTextureHeight = backgroundTextureHeight * backgroundScale;

    // Create a vertex array for the background
    sf::VertexArray backgroundVertices(sf::Quads, 4);

    // Calculate positions to fill the window
    backgroundVertices[0].position = sf::Vector2f(0.0f, 0.0f);
    backgroundVertices[1].position = sf::Vector2f(static_cast<float>(windowSize.x), 0.0f);
    backgroundVertices[2].position = sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
    backgroundVertices[3].position = sf::Vector2f(0.0f, static_cast<float>(windowSize.y));

    // Adjust texture coordinates to account for scaling
    float texCoordOffsetX = backgroundOffsetX / backgroundScale;
    float texCoordWidth = static_cast<float>(windowSize.x) / backgroundScale;

    backgroundVertices[0].texCoords = sf::Vector2f(texCoordOffsetX, 0.0f);
    backgroundVertices[1].texCoords = sf::Vector2f(texCoordOffsetX + texCoordWidth, 0.0f);
    backgroundVertices[2].texCoords = sf::Vector2f(texCoordOffsetX + texCoordWidth, backgroundTextureHeight);
    backgroundVertices[3].texCoords = sf::Vector2f(texCoordOffsetX, backgroundTextureHeight);

    // Draw the background
    sf::RenderStates bgStates;
    bgStates.texture = &backgroundTexture;
    window.draw(backgroundVertices, bgStates);

    // --- MIDDLEGROUND RENDERING WITH ADJUSTED SCALING AND CONSISTENT SCROLLING ---

    // Set the middleground texture to repeat horizontally
    middlegroundTexture.setRepeated(true);

    // Parallax factor and scrolling speed for middleground
    float middlegroundParallaxFactor = 0.1f;
    static float middlegroundScrollOffset = 0.0f;
    float middlegroundScrollSpeed = 6.0f;

    // Update middleground scrolling based on deltaTime
    middlegroundScrollOffset += middlegroundScrollSpeed * deltaTime;

    // Calculate middleground offset
    float middlegroundOffsetX = playerX * middlegroundParallaxFactor + middlegroundScrollOffset;

    // Calculate scaling factor based on desired height
    float middlegroundTextureWidth = static_cast<float>(middlegroundTexture.getSize().x);
    float middlegroundTextureHeight = static_cast<float>(middlegroundTexture.getSize().y);

    // Set a desired height for the middleground relative to the window height
    float desiredMiddlegroundHeight = windowSize.y * 0.5f; // Adjust as needed
    float middlegroundScale = desiredMiddlegroundHeight / middlegroundTextureHeight;

    // Adjusted texture dimensions after scaling
    float scaledMiddlegroundWidth = middlegroundTextureWidth * middlegroundScale;
    float scaledMiddlegroundHeight = desiredMiddlegroundHeight;

    // Create a vertex array for the middleground
    sf::VertexArray middlegroundVertices(sf::Quads, 4);

    // Set the vertical position where you want the middleground to appear
    float middlegroundPositionY = windowSize.y - scaledMiddlegroundHeight - 50.0f; // Adjust the offset as needed

    // Set up the quad covering the desired middleground area
    middlegroundVertices[0].position = sf::Vector2f(0.0f, middlegroundPositionY);
    middlegroundVertices[1].position = sf::Vector2f(static_cast<float>(windowSize.x), middlegroundPositionY);
    middlegroundVertices[2].position = sf::Vector2f(static_cast<float>(windowSize.x), middlegroundPositionY + scaledMiddlegroundHeight);
    middlegroundVertices[3].position = sf::Vector2f(0.0f, middlegroundPositionY + scaledMiddlegroundHeight);

    // Adjust texture coordinates to account for scaling and ensure consistent scrolling
    float texCoordScale = middlegroundScale;

    // Wrap the texture coordinate offset to prevent flipping or resetting
    float textureCoordOffsetX = fmod(middlegroundOffsetX / texCoordScale, middlegroundTextureWidth);
    if (textureCoordOffsetX < 0) textureCoordOffsetX += middlegroundTextureWidth;

    float textureCoordWidth = static_cast<float>(windowSize.x) / texCoordScale;

    middlegroundVertices[0].texCoords = sf::Vector2f(textureCoordOffsetX, 0.0f);
    middlegroundVertices[1].texCoords = sf::Vector2f(textureCoordOffsetX + textureCoordWidth, 0.0f);
    middlegroundVertices[2].texCoords = sf::Vector2f(textureCoordOffsetX + textureCoordWidth, middlegroundTextureHeight);
    middlegroundVertices[3].texCoords = sf::Vector2f(textureCoordOffsetX, middlegroundTextureHeight);

    // Draw the middleground
    sf::RenderStates mgStates;
    mgStates.texture = &middlegroundTexture;
    window.draw(middlegroundVertices, mgStates);
}

