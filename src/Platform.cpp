#include "../include/Platform.hpp"
#include <cmath>

Platform::Platform(float x, float y, float width, float height, const sf::Texture& texture, bool isGrassy) {
    float tileWidth = static_cast<float>(texture.getSize().x);
    float tileHeight = static_cast<float>(texture.getSize().y);

    int numTilesX = static_cast<int>(std::ceil(width / tileWidth));
    int numTilesY = static_cast<int>(std::ceil(height / tileHeight));

    for (int i = 0; i < numTilesX; ++i) {
        for (int j = 0; j < numTilesY; ++j) {
            sf::RectangleShape tile(sf::Vector2f(tileWidth, tileHeight));
            tile.setPosition(x + i * tileWidth, y + j * tileHeight);
            tile.setTexture(&texture);
            tile.setTextureRect(sf::IntRect(0, 0, static_cast<int>(tileWidth), static_cast<int>(tileHeight)));
            tiles.push_back(tile);  
        }
    }

    // If this is a grassy block, make only the bottom part solid
    if (isGrassy) {
        float solidHeight = tileHeight - 16; // Only bottom part is solid, top 16px is grass
        collisionBounds = sf::FloatRect(x, y + 16, width, solidHeight);  // Set bounds starting after 16px of grass
    } else {
        collisionBounds = sf::FloatRect(x, y, width, height);  // Normal collision bounds
    }
}

sf::FloatRect Platform::getBounds() const {
    if (tiles.empty()) {
        return sf::FloatRect();  
    }

    sf::FloatRect firstTileBounds = tiles.front().getGlobalBounds();
    sf::FloatRect lastTileBounds = tiles.back().getGlobalBounds();

    float left = firstTileBounds.left;
    float top = firstTileBounds.top;
    float width = (lastTileBounds.left + lastTileBounds.width) - left;
    float height = (lastTileBounds.top + lastTileBounds.height) - top;

    return sf::FloatRect(left, top, width, height);
}

const std::vector<sf::RectangleShape>& Platform::getTiles() const {
    return tiles;  
}

void Platform::draw(sf::RenderWindow& window) {
    for (const auto& tile : tiles) {
        window.draw(tile);  
    }
}

// Rescale the platform when window is resized
void Platform::rescale(float scaleX, float scaleY) {
    for (auto& tile : tiles) {
        tile.setScale(scaleX, scaleY);
    }
}

// Collision check function
bool Platform::checkCollision(const sf::FloatRect& playerBounds) const {
    return collisionBounds.intersects(playerBounds);  // Use the modified collision bounds
}

