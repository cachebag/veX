#include "../include/Platform.hpp"
#include <cmath>

Platform::Platform(float x, float y, float width, float height, const sf::Texture& texture) {
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

void Platform::rescale(float scaleX, float scaleY) {
    for (auto& tile : tiles) {
        tile.setScale(scaleX, scaleY);
    }
}

