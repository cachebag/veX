#include "../include/Platform.hpp"

// Constructor initializes the platform and sets up tiled sections
Platform::Platform(float x, float y, float width, float height, sf::Texture& texture, const sf::Vector2u& windowSize) {
    // Enable texture repetition
    texture.setRepeated(true);

    // Calculate the number of tiles needed to cover the platform size
    float tileWidth = static_cast<float>(texture.getSize().x);
    float tileHeight = static_cast<float>(texture.getSize().y);

    float platformWidth = width * windowSize.x;
    float platformHeight = height * windowSize.y;

    int numTilesX = static_cast<int>(std::ceil(platformWidth / tileWidth));
    int numTilesY = static_cast<int>(std::ceil(platformHeight / tileHeight));

    // Create and position the tiles
    for (int i = 0; i < numTilesX; ++i) {
        for (int j = 0; j < numTilesY; ++j) {
            sf::RectangleShape tile(sf::Vector2f(tileWidth, tileHeight));
            tile.setPosition(x * windowSize.x + i * tileWidth, y * windowSize.y + j * tileHeight);
            tile.setTexture(&texture);

            tiles.push_back(tile);  // Store each tile for later rendering
        }
    }
}

// Get the bounding box of the entire platform (used for general bounding box collision)
sf::FloatRect Platform::getBounds() const {
    if (tiles.empty()) {
        return sf::FloatRect();  // Return empty bounds if no tiles exist
    }

    // Get the bounds of the first and last tile to create a bounding box for the entire platform
    sf::FloatRect firstTileBounds = tiles.front().getGlobalBounds();
    sf::FloatRect lastTileBounds = tiles.back().getGlobalBounds();

    float left = firstTileBounds.left;
    float top = firstTileBounds.top;
    float width = lastTileBounds.left + lastTileBounds.width - left;
    float height = lastTileBounds.top + lastTileBounds.height - top;

    return sf::FloatRect(left, top, width, height);
}

// Get individual tiles for more precise collision detection
const std::vector<sf::RectangleShape>& Platform::getTiles() const {
    return tiles;  // Return reference to the tile array for collision checks
}

// Draw the platform by rendering each individual tile
void Platform::draw(sf::RenderWindow& window) {
    for (const auto& tile : tiles) {
        window.draw(tile);  // Draw each tile
    }
}

