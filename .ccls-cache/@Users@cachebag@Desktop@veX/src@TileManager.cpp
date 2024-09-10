#include "../include/TileManager.hpp"
#include <fstream>
#include <iostream>

TileManager::TileManager() {
    loadTextures();
    
    // Initialize an empty 100x100 grid for tile placement (you can adjust this)
    levelGrid.resize(100, std::vector<TileType>(100, TileType::None));
}

void TileManager::loadTextures() {
    if (!groundTexture.loadFromFile("assets/tutorial_level/ground.png") ||
        !ground2Texture.loadFromFile("assets/tutorial_level/ground2.png") ||
        !ground3Texture.loadFromFile("assets/tutorial_level/ground3.png")) {
        std::cerr << "Error loading tile textures!" << std::endl;
    }

    tileTextures[TileType::Ground] = &groundTexture;
    tileTextures[TileType::Ground2] = &ground2Texture;
    tileTextures[TileType::Ground3] = &ground3Texture;
}

void TileManager::draw(sf::RenderWindow& window) {
    // Draw all placed tiles
    for (auto& tile : tiles) {
        window.draw(tile);
    }
    
    if (debugMode) {
        drawGrid(window); // Optional: Draw gridlines for debugging
    }
}

void TileManager::handleInput(sf::RenderWindow& window) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        selectedTile = TileType::Ground;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        selectedTile = TileType::Ground2;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
        selectedTile = TileType::Ground3;
    }
    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // Get mouse position and convert it to the grid position
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int gridX = mousePos.x / tileSize;
        int gridY = mousePos.y / tileSize;

        if (gridX >= 0 && gridY >= 0 && gridX < levelGrid.size() && gridY < levelGrid[0].size()) {
            levelGrid[gridX][gridY] = selectedTile;

            // Create a visual representation (a tile) to display
            sf::RectangleShape tileShape(sf::Vector2f(tileSize, tileSize));
            tileShape.setPosition(gridX * tileSize, gridY * tileSize);
            tileShape.setTexture(tileTextures[selectedTile]);

            tiles.push_back(tileShape); // Add it to the list of placed tiles
        }
    }
}

void TileManager::saveLevel(const std::string& filename) {
    std::ofstream levelFile(filename);
    if (!levelFile) {
        std::cerr << "Error saving level!" << std::endl;
        return;
    }

    // Save the grid to a text file
    for (const auto& row : levelGrid) {
        for (const auto& tile : row) {
            levelFile << static_cast<int>(tile) << " "; // Convert TileType enum to an integer
        }
        levelFile << "\n";
    }

    levelFile.close();
}

void TileManager::loadLevel(const std::string& filename) {
    std::ifstream levelFile(filename);
    if (!levelFile) {
        std::cerr << "Error loading level!" << std::endl;
        return;
    }

    int tileType;
    for (int i = 0; i < levelGrid.size(); ++i) {
        for (int j = 0; j < levelGrid[i].size(); ++j) {
            levelFile >> tileType;
            levelGrid[i][j] = static_cast<TileType>(tileType);

            if (levelGrid[i][j] != TileType::None) {
                // Visualize the tile
                sf::RectangleShape tileShape(sf::Vector2f(tileSize, tileSize));
                tileShape.setPosition(i * tileSize, j * tileSize);
                tileShape.setTexture(tileTextures[levelGrid[i][j]]);
                tiles.push_back(tileShape);
            }
        }
    }

    levelFile.close();
}

void TileManager::toggleDebugMode() {
    debugMode = !debugMode;
}

void TileManager::drawGrid(sf::RenderWindow& window) {
    sf::RectangleShape line;
    line.setFillColor(sf::Color::Green);

    for (int x = 0; x < window.getSize().x; x += tileSize) {
        line.setSize(sf::Vector2f(1, window.getSize().y));
        line.setPosition(x, 0);
        window.draw(line);
    }

    for (int y = 0; y < window.getSize().y; y += tileSize) {
        line.setSize(sf::Vector2f(window.getSize().x, 1));
        line.setPosition(0, y);
        window.draw(line);
    }
}

