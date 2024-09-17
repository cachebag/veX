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
    // --- RENDER BACKGROUND WITH ZOOM AND VERY SUBTLE MOVEMENT ---

    // Set the background texture to repeat to avoid edge issues
    backgroundTexture.setRepeated(true);

    // Zoom factor for the background, simulating distance
    float zoomFactor = 0.7f;  // Adjust this to zoom the background as needed
    sf::View zoomedView = window.getDefaultView();
    zoomedView.zoom(zoomFactor);

    // Set the zoomed view for the background
    window.setView(zoomedView);

    // Subtle parallax factor for far background movement
    float backgroundParallaxFactor = 0.1f;  // Small value for barely noticeable movement

    // Calculate background offset using player's X position (very subtle movement)
    float backgroundOffsetX = playerX * backgroundParallaxFactor;

    // Use modulo to wrap texture (for extreme edge cases)
    float textureWidth = static_cast<float>(backgroundTexture.getSize().x);
    backgroundOffsetX = fmod(backgroundOffsetX, textureWidth);

    // Snap the background offset to the nearest integer to avoid sub-pixel rendering issues
    int integerBackgroundOffsetX = static_cast<int>(std::round(backgroundOffsetX));

    // Set the texture rectangle for the background, ensuring it fits the window size
    backgroundSprite.setTextureRect(sf::IntRect(integerBackgroundOffsetX, 0, windowSize.x, backgroundTexture.getSize().y));

    // Scale the background to fit the window size
    float scaleX = static_cast<float>(windowSize.x) / backgroundTexture.getSize().x;
    float scaleY = static_cast<float>(windowSize.y) / backgroundTexture.getSize().y;
    backgroundSprite.setScale(scaleX, scaleY);

    // Draw the zoomed and subtly moving background
    window.draw(backgroundSprite);

    // --- RESET VIEW TO DEFAULT BEFORE RENDERING MIDDLEGROUND ---
    window.setView(window.getDefaultView());

    // --- MIDDLEGROUND RENDERING ---
    middlegroundTexture.setRepeated(true);

    float middlegroundParallaxFactor = 0.2f;
    static float middlegroundScrollSpeed = 5.0f;  // Slower scrolling speed for smoother movement
    static float middlegroundScrollOffset = 0.0f;  // Offset for scrolling

    // Update middleground scrolling based on deltaTime
    middlegroundScrollOffset += middlegroundScrollSpeed * deltaTime;

    // Calculate middleground offset, incorporating parallax and scroll speed
    float middlegroundOffsetX = playerX * middlegroundParallaxFactor + middlegroundScrollOffset;
    float middleTextureWidth = static_cast<float>(middlegroundTexture.getSize().x);
    middlegroundOffsetX = fmod(middlegroundOffsetX, middleTextureWidth);

    // Snap the middleground offset to the nearest integer to avoid sub-pixel issues
    int integerMiddlegroundOffsetX = static_cast<int>(std::round(middlegroundOffsetX));

    // Set the texture rectangle using the snapped value
    middlegroundSprite.setTextureRect(sf::IntRect(integerMiddlegroundOffsetX, 0, windowSize.x, middlegroundTexture.getSize().y));

    // Calculate aspect ratio and scale
    sf::Vector2u middleTextureSize = middlegroundTexture.getSize();
    float aspectRatio = static_cast<float>(middleTextureSize.x) / middleTextureSize.y;
    float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;

    float middleScaleX, middleScaleY;
    if (windowAspectRatio > aspectRatio) {
        middleScaleY = static_cast<float>(windowSize.y) / middleTextureSize.y;
        middleScaleX = middleScaleY;  // Maintain aspect ratio
    } else {
        middleScaleX = static_cast<float>(windowSize.x) / middleTextureSize.x;
        middleScaleY = middleScaleX;  // Maintain aspect ratio
    }

    // Snap scale values to the nearest integers to avoid floating-point scaling issues
    middleScaleX = static_cast<int>(middleScaleX);
    middleScaleY = static_cast<int>(middleScaleY);
    middlegroundSprite.setScale(middleScaleX, middleScaleY);

    // Calculate position and snap to avoid sub-pixel rendering issues
    float middlePositionY = (windowSize.y - middleTextureSize.y * middleScaleY) / 2.0f;
    middlegroundSprite.setPosition(0.0f, middlePositionY + 250.0f);
    middlegroundSprite.setPosition(static_cast<int>(middlegroundSprite.getPosition().x), static_cast<int>(middlegroundSprite.getPosition().y));

    // Draw the middleground
    window.draw(middlegroundSprite);
}

