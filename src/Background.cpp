#include "../include/Background.hpp"
#include <iostream>
#include <cmath>

Background::Background(const std::string& backgroundFilePath, const std::string& middlegroundFilePath, const std::string& mountainsFilePath, const sf::Vector2u& windowSize) {

    if (!backgroundTexture.loadFromFile(backgroundFilePath)) {
        std::cerr << "Error loading background texture from " << backgroundFilePath << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);

    if (!middlegroundTexture.loadFromFile(middlegroundFilePath)) {
        std::cerr << "Error loading middleground texture from " << middlegroundFilePath << std::endl;
    }

    if (!mountainsTexture.loadFromFile(mountainsFilePath)) {
        std::cerr << "Error loading mountains texture from " << mountainsFilePath << std::endl;
    }
    middlegroundTexture.setRepeated(true);

    middlegroundSprite.setTextureRect(sf::IntRect(0, 0, windowSize.x, middlegroundTexture.getSize().y));

    middlegroundSprite.setTexture(middlegroundTexture);

    mountainsTexture.setRepeated(true);

    mountainsSprite.setTextureRect(sf::IntRect(0, 0, windowSize.x, mountainsTexture.getSize().y));

    mountainsSprite.setTexture(mountainsTexture);
}

void Background::render(sf::RenderWindow& window, const sf::Vector2u& windowSize, float playerX, float deltaTime) {

    backgroundTexture.setRepeated(true);

    float backgroundParallaxFactor = 0.01f;

    float backgroundOffsetX = playerX * backgroundParallaxFactor;

    float backgroundTextureWidth = static_cast<float>(backgroundTexture.getSize().x);
    backgroundOffsetX = fmod(backgroundOffsetX, backgroundTextureWidth);
    if (backgroundOffsetX < 0) backgroundOffsetX += backgroundTextureWidth;

    float backgroundTextureHeight = static_cast<float>(backgroundTexture.getSize().y);
    float backgroundAspectRatio = backgroundTextureWidth / backgroundTextureHeight;
    float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;

    float backgroundScale;
    if (windowAspectRatio > backgroundAspectRatio) {
        backgroundScale = static_cast<float>(windowSize.y) / backgroundTextureHeight;
    } else {
        backgroundScale = static_cast<float>(windowSize.x) / backgroundTextureWidth;
    }

    sf::VertexArray backgroundVertices(sf::Quads, 4);

    backgroundVertices[0].position = sf::Vector2f(0.0f, 0.0f);
    backgroundVertices[1].position = sf::Vector2f(static_cast<float>(windowSize.x), 0.0f);
    backgroundVertices[2].position = sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
    backgroundVertices[3].position = sf::Vector2f(0.0f, static_cast<float>(windowSize.y));

    float texCoordOffsetX = backgroundOffsetX / backgroundScale;
    float texCoordWidth = static_cast<float>(windowSize.x) / backgroundScale;

    backgroundVertices[0].texCoords = sf::Vector2f(texCoordOffsetX, 0.0f);
    backgroundVertices[1].texCoords = sf::Vector2f(texCoordOffsetX + texCoordWidth, 0.0f);
    backgroundVertices[2].texCoords = sf::Vector2f(texCoordOffsetX + texCoordWidth, backgroundTextureHeight);
    backgroundVertices[3].texCoords = sf::Vector2f(texCoordOffsetX, backgroundTextureHeight);

    sf::RenderStates bgStates;
    bgStates.texture = &backgroundTexture;
    window.draw(backgroundVertices, bgStates);

    mountainsTexture.setRepeated(true);

    float mountainsParallaxFactor = 0.1f;
    static float mountainsScrollOffset = 0.0f;
    float mountainsScrollSpeed = 4.0f;

    mountainsScrollOffset += mountainsScrollSpeed * deltaTime;

    float mountainsOffsetX = playerX * mountainsParallaxFactor + mountainsScrollOffset;

    float mountainsTextureWidth = static_cast<float>(mountainsTexture.getSize().x);
    float mountainsTextureHeight = static_cast<float>(mountainsTexture.getSize().y);

    float desiredMountainsHeight = windowSize.y * 0.5f;
    float mountainsScale = desiredMountainsHeight / mountainsTextureHeight;

    sf::VertexArray mountainsVertices(sf::Quads, 4);

    float mountainsPositionY = windowSize.y - desiredMountainsHeight - 100.0f;

    mountainsVertices[0].position = sf::Vector2f(0.0f, mountainsPositionY);
    mountainsVertices[1].position = sf::Vector2f(static_cast<float>(windowSize.x), mountainsPositionY);
    mountainsVertices[2].position = sf::Vector2f(static_cast<float>(windowSize.x), mountainsPositionY + desiredMountainsHeight);
    mountainsVertices[3].position = sf::Vector2f(0.0f, mountainsPositionY + desiredMountainsHeight);

    float mTexCoordScale = mountainsScale;

    float mTexCoordOffsetX = fmod(mountainsOffsetX / mTexCoordScale, mountainsTextureWidth);
    if (mTexCoordOffsetX < 0) mTexCoordOffsetX += mountainsTextureWidth;

    float mTexCoordWidth = static_cast<float>(windowSize.x) / mTexCoordScale;

    mountainsVertices[0].texCoords = sf::Vector2f(mTexCoordOffsetX, 0.0f);
    mountainsVertices[1].texCoords = sf::Vector2f(mTexCoordOffsetX + mTexCoordWidth, 0.0f);
    mountainsVertices[2].texCoords = sf::Vector2f(mTexCoordOffsetX + mTexCoordWidth, mountainsTextureHeight);
    mountainsVertices[3].texCoords = sf::Vector2f(mTexCoordOffsetX, mountainsTextureHeight);

    sf::RenderStates mountStates;
    mountStates.texture = &mountainsTexture;
    window.draw(mountainsVertices, mountStates);

    middlegroundTexture.setRepeated(true);

    float middlegroundParallaxFactor = 0.3f;
    static float middlegroundScrollOffset = 0.0f;
    float middlegroundScrollSpeed = 0.5f;

    middlegroundScrollOffset += middlegroundScrollSpeed * deltaTime;

    float middlegroundOffsetX = playerX * middlegroundParallaxFactor + middlegroundScrollOffset;

    float middlegroundTextureWidth = static_cast<float>(middlegroundTexture.getSize().x);
    float middlegroundTextureHeight = static_cast<float>(middlegroundTexture.getSize().y);

    float desiredMiddlegroundHeight = windowSize.y * 0.5f;
    float middlegroundScale = desiredMiddlegroundHeight / middlegroundTextureHeight;

    sf::VertexArray middlegroundVertices(sf::Quads, 4);

    float middlegroundPositionY = windowSize.y - desiredMiddlegroundHeight - 1.0f;

    middlegroundVertices[0].position = sf::Vector2f(0.0f, middlegroundPositionY);
    middlegroundVertices[1].position = sf::Vector2f(static_cast<float>(windowSize.x), middlegroundPositionY);
    middlegroundVertices[2].position = sf::Vector2f(static_cast<float>(windowSize.x), middlegroundPositionY + desiredMiddlegroundHeight);
    middlegroundVertices[3].position = sf::Vector2f(0.0f, middlegroundPositionY + desiredMiddlegroundHeight);

    float texCoordScale = middlegroundScale;

    float textureCoordOffsetX = fmod(middlegroundOffsetX / texCoordScale, middlegroundTextureWidth);
    if (textureCoordOffsetX < 0) textureCoordOffsetX += middlegroundTextureWidth;

    float textureCoordWidth = static_cast<float>(windowSize.x) / texCoordScale;

    middlegroundVertices[0].texCoords = sf::Vector2f(textureCoordOffsetX, 0.0f);
    middlegroundVertices[1].texCoords = sf::Vector2f(textureCoordOffsetX + textureCoordWidth, 0.0f);
    middlegroundVertices[2].texCoords = sf::Vector2f(textureCoordOffsetX + textureCoordWidth, middlegroundTextureHeight);
    middlegroundVertices[3].texCoords = sf::Vector2f(textureCoordOffsetX, middlegroundTextureHeight);

    sf::RenderStates mgStates;
    mgStates.texture = &middlegroundTexture;
    window.draw(middlegroundVertices, mgStates);
}

