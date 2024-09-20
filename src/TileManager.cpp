#include "../include/TileManager.hpp"
#include <fstream>
#include <iostream>
#include "../include/nfd.h"

// Tile Constructor
Tile::Tile(TileType type, const sf::Vector2f& position, const sf::Texture& texture, const TileProperties& properties)
    : type(type), properties(properties) {
    shape.setSize(properties.size);
    shape.setPosition(position);
    shape.setTexture(&texture);
}

// TileManager Constructor
TileManager::TileManager() {
    loadTextures();
    setupTileProperties();
    levelGrid.resize(100, std::vector<TileType>(100, TileType::None));  // 100x100 grid for tile placement
}

// Load all tile textures
void TileManager::loadTextures() {
    // Load each texture and associate it with a TileType
    std::map<TileType, std::string> textureFiles = {
        {TileType::Brick, "assets/tutorial_level/brick.png"},
        {TileType::BushLarge, "assets/tutorial_level/bush-large.png"},
        {TileType::BushSmall, "assets/tutorial_level/bush-small.png"},
        {TileType::SlimeBlock, "assets/tutorial_level/slime-block.png"},
        {TileType::Statue, "assets/tutorial_level/statue.png"},
        {TileType::Stone1, "assets/tutorial_level/stone-1.png"},
        {TileType::Stone2, "assets/tutorial_level/stone-2.png"},
        {TileType::Stone3, "assets/tutorial_level/stone-3.png"},
        {TileType::Stone4, "assets/tutorial_level/stone-4.png"},
        {TileType::Tree1, "assets/tutorial_level/tree-1.png"},
        {TileType::Tree2, "assets/tutorial_level/tree-2.png"},
        {TileType::Tree3, "assets/tutorial_level/tree-3.png"},
        {TileType::Rock, "assets/tutorial_level/rock.png"}
    };

    for (const auto& [type, file] : textureFiles) {
        if (!tileTextures[type].loadFromFile(file)) {
            std::cerr << "Error loading texture: " << file << std::endl;
        }
    }
}

// Setup tile properties for different tile types
void TileManager::setupTileProperties() {
    tileProperties[TileType::Brick] = TileProperties(true, {64, 64}, false);
    tileProperties[TileType::BushLarge] = TileProperties(false, {128, 128}, false);
    tileProperties[TileType::BushSmall] = TileProperties(false, {64, 64}, false);
    tileProperties[TileType::SlimeBlock] = TileProperties(true, {64, 64}, true);
    tileProperties[TileType::Statue] = TileProperties(true, {64, 128}, false);
    tileProperties[TileType::Stone1] = TileProperties(true, {64, 64}, true);
    tileProperties[TileType::Stone2] = TileProperties(true, {64, 64}, true);
    tileProperties[TileType::Stone3] = TileProperties(true, {64, 64}, true);
    tileProperties[TileType::Stone4] = TileProperties(true, {64, 64}, true);
    tileProperties[TileType::Tree1] = TileProperties(false, {64, 192}, false);
    tileProperties[TileType::Tree2] = TileProperties(false, {64, 192}, false);
    tileProperties[TileType::Tree3] = TileProperties(false, {64, 192}, false);
    tileProperties[TileType::Rock] = TileProperties(true, {64, 64}, false);
}

// Save the level to a file
void TileManager::saveLevel() {
    nfdchar_t* savePath = nullptr;
    nfdresult_t result = NFD_SaveDialog("txt", nullptr, &savePath);

    if (result == NFD_OKAY) {
        std::ofstream levelFile(savePath);
        if (!levelFile) {
            std::cerr << "Error saving level!" << std::endl;
            return;
        }

        for (const auto& row : levelGrid) {
            for (const auto& tile : row) {
                levelFile << static_cast<int>(tile) << " ";
            }
            levelFile << "\n";
        }

        levelFile.close();
        std::cout << "Level saved to: " << savePath << std::endl;
        free(savePath);
    } else if (result == NFD_CANCEL) {
        std::cout << "Save canceled." << std::endl;
    } else {
        std::cerr << "Error opening save dialog." << std::endl;
    }
}

// Load a level from a file
void TileManager::loadLevel() {
    nfdchar_t* loadPath = nullptr;
    nfdresult_t result = NFD_OpenDialog("txt", nullptr, &loadPath);

    if (result == NFD_OKAY) {
        std::ifstream levelFile(loadPath);
        if (!levelFile) {
            std::cerr << "Error loading level!" << std::endl;
            return;
        }

        int tileType;
        tiles.clear();  // Clear existing tiles

        for (size_t i = 0; i < levelGrid.size(); ++i) {
            for (size_t j = 0; j < levelGrid[i].size(); ++j) {
                levelFile >> tileType;
                levelGrid[i][j] = static_cast<TileType>(tileType);

                if (levelGrid[i][j] != TileType::None) {
                    sf::Vector2f position(i * tileSize, j * tileSize);
                    const sf::Texture& texture = tileTextures[levelGrid[i][j]];
                    const TileProperties& properties = tileProperties[levelGrid[i][j]];

                    // Create a new Tile object
                    tiles.emplace_back(levelGrid[i][j], position, texture, properties);
                }
            }
        }

        levelFile.close();
        std::cout << "Level loaded from: " << loadPath << std::endl;
        free(loadPath);
    } else if (result == NFD_CANCEL) {
        std::cout << "Load canceled." << std::endl;
    } else {
        std::cerr << "Error opening load dialog." << std::endl;
    }
}

// Toggle debug mode to show grid lines
void TileManager::toggleDebugMode() {
    debugMode = !debugMode;
}

// Draw the grid lines for the level editor
void TileManager::drawGrid(sf::RenderWindow& window) {
    sf::RectangleShape line;
    line.setFillColor(sf::Color::Green);

    for (unsigned int x = 0; x < window.getSize().x; x += tileSize) {
        line.setSize(sf::Vector2f(1, static_cast<float>(window.getSize().y)));
        line.setPosition(static_cast<float>(x), 0);
        window.draw(line);
    }

    for (unsigned int y = 0; y < window.getSize().y; y += tileSize) {
        line.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), 1));
        line.setPosition(0, static_cast<float>(y));
        window.draw(line);
    }
}

// Handle user input for placing/removing tiles
void TileManager::handleInput(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    int gridX = mousePos.x / tileSize;
    int gridY = mousePos.y / tileSize;

    // Remove sidebar restriction check

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (gridX >= 0 && gridY >= 0 && static_cast<size_t>(gridX) < levelGrid.size() && static_cast<size_t>(gridY) < levelGrid[0].size()) {
            levelGrid[gridX][gridY] = selectedTile;

            sf::Vector2f position(gridX * tileSize, gridY * tileSize);
            const sf::Texture& texture = tileTextures[selectedTile];
            const TileProperties& properties = tileProperties[selectedTile];

            // Create a new Tile object
            tiles.emplace_back(selectedTile, position, texture, properties);
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        if (gridX >= 0 && gridY >= 0 && static_cast<size_t>(gridX) < levelGrid.size() && static_cast<size_t>(gridY) < levelGrid[0].size()) {
            auto it = std::remove_if(tiles.begin(), tiles.end(), [&](const Tile& tile) {
                return tile.shape.getPosition().x == gridX * tileSize && tile.shape.getPosition().y == gridY * tileSize;
            });

            if (it != tiles.end()) {
                tiles.erase(it, tiles.end());
            }

            levelGrid[gridX][gridY] = TileType::None;
        }
    }
}

// Draw a preview of the currently selected tile under the mouse cursor
void TileManager::drawTilePreview(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::RectangleShape preview(sf::Vector2f(static_cast<float>(tileSize), static_cast<float>(tileSize)));
    preview.setPosition(static_cast<float>(mousePos.x - mousePos.x % tileSize), static_cast<float>(mousePos.y - mousePos.y % tileSize));
    preview.setTexture(&tileTextures[selectedTile]);
    window.draw(preview);
}

// Set the currently selected tile from Sidebar
void TileManager::setSelectedTile(TileType type) {
    selectedTile = type;
}

// Draw all the tiles placed on the level
void TileManager::draw(sf::RenderWindow& window) {
    for (const auto& tile : tiles) {
        window.draw(tile.shape);
    }

    if (debugMode) {
        drawGrid(window);
    }
}

// Get the placed tiles
const std::vector<Tile>& TileManager::getPlacedTiles() const {
    return tiles;
}

// Get the tile textures
const std::map<TileType, sf::Texture>& TileManager::getTileTextures() const {
    return tileTextures;
}

