#include "../include/TileManager.hpp"
#include <fstream>
#include <iostream>

TileManager::TileManager() {
    loadTextures();
    levelGrid.resize(100, std::vector<TileType>(100, TileType::None)); // 100x100 grid for tile placement
}

void TileManager::loadTextures() {
    if (!groundTexture.loadFromFile("assets/tutorial_level/main_ground_t1.png") ||
        !ground2Texture.loadFromFile("assets/tutorial_level/main_ground_t2.png") ||
        !ground3Texture.loadFromFile("assets/tutorial_level/main_ground_t3.png") ||
        !rubbleTexture.loadFromFile("assets/tutorial_level/rubble.png") ||  // Load new textures
        !undergroundTexture.loadFromFile("assets/tutorial_level/underground.png") ||
        !naturalTexture.loadFromFile("assets/tutorial_level/natural.png")) {
        std::cerr << "Error loading tile textures!" << std::endl;
    }

    tileTextures[TileType::Ground] = &groundTexture;
    tileTextures[TileType::Ground2] = &ground2Texture;
    tileTextures[TileType::Ground3] = &ground3Texture;
    tileTextures[TileType::Rubble] = &rubbleTexture;        // Assign new textures
    tileTextures[TileType::Underground] = &undergroundTexture;
    tileTextures[TileType::Natural] = &naturalTexture;
}

void TileManager::openTileSelectorPopup(sf::RenderWindow& mainWindow) {
    // Create a pop-up window for selecting tiles
    sf::RenderWindow popupWindow(sf::VideoMode(300, 800), "Tile Selector", sf::Style::Titlebar | sf::Style::Close);
    
    while (popupWindow.isOpen()) {
        sf::Event event;
        while (popupWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                popupWindow.close();  // Close the pop-up window
            }
        }

        popupWindow.clear();
        
        sf::RectangleShape tilePreview(sf::Vector2f(tileSize, tileSize));
        float x = 50.0f;
        float y = 50.0f;

        // Loop through all tiles in tileTextures map and display them
        for (const auto& [tileType, texture] : tileTextures) {
            tilePreview.setPosition(x, y);
            tilePreview.setTexture(texture);
            popupWindow.draw(tilePreview);

            // Check for tile selection
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(popupWindow);
                if (tilePreview.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    selectedTile = tileType;  // Set selected tile
                    popupWindow.close();  // Close the pop-up window
                }
            }

            y += tileSize + 10.0f;
        }

        popupWindow.display();
    }
}

void TileManager::draw(sf::RenderWindow& window) {
    for (auto& tile : tiles) {
        window.draw(tile);
    }

    if (debugMode) {
        drawGrid(window); 
    }
}

void TileManager::drawTileSelector(sf::RenderWindow& window) {
    sf::RectangleShape tilePreview(sf::Vector2f(tileSize, tileSize));
    
    float x = 200.0f;  // Margin for UI on the left side
    float y = 200.0f;  // Initial Y position for tile selector

    // Loop through all tiles in tileTextures map and display them in the selector
    for (const auto& [tileType, texture] : tileTextures) {
        tilePreview.setPosition(x, y);
        tilePreview.setTexture(texture);
        tilePreview.setOutlineThickness(2);
        tilePreview.setOutlineColor(tileType == selectedTile ? sf::Color::Red : sf::Color::Transparent);
        window.draw(tilePreview);

        y += tileSize + 10.0f;  // Move down for the next tile in the menu
    }
}

void TileManager::drawTilePreview(sf::RenderWindow& window) {
    sf::RectangleShape preview(sf::Vector2f(tileSize, tileSize));
    preview.setTexture(tileTextures[selectedTile]);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    preview.setPosition(static_cast<float>(mousePos.x - mousePos.x % tileSize), static_cast<float>(mousePos.y - mousePos.y % tileSize));

    window.draw(preview);
}

void TileManager::drawSaveLoadButtons(sf::RenderWindow& window) {
    sf::RectangleShape saveButton(sf::Vector2f(150, 50));
    saveButton.setPosition(window.getSize().x - 350, 20);  // Positioned at top-right
    saveButton.setFillColor(sf::Color::Blue);
    saveButton.setOutlineThickness(2);
    saveButton.setOutlineColor(sf::Color::White);

    sf::RectangleShape loadButton(sf::Vector2f(150, 50));
    loadButton.setPosition(window.getSize().x - 180, 20);  // Positioned at top-right next to save button
    loadButton.setFillColor(sf::Color::Green);
    loadButton.setOutlineThickness(2);
    loadButton.setOutlineColor(sf::Color::White);

    window.draw(saveButton);
    window.draw(loadButton);

    sf::Font font;
    font.loadFromFile("assets/fonts/Merriweather-Regular.ttf");
    sf::Text saveText("Save", font, 20);
    saveText.setPosition(window.getSize().x - 320, 30);
    window.draw(saveText);

    sf::Text loadText("Load", font, 20);
    loadText.setPosition(window.getSize().x - 150, 30);
    window.draw(loadText);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (saveButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            saveLevel("levels/level1.txt");
        }

        if (loadButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            loadLevel("levels/level1.txt");
        }
    }
}

void TileManager::handleInput(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    int gridX = mousePos.x / tileSize;
    int gridY = mousePos.y / tileSize;

    // If left mouse button is pressed, place a tile
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (gridX >= 0 && gridY >= 0 && static_cast<size_t>(gridX) < levelGrid.size() && static_cast<size_t>(gridY) < levelGrid[0].size()) {
            levelGrid[gridX][gridY] = selectedTile;

            sf::RectangleShape tileShape(sf::Vector2f(tileSize, tileSize));
            tileShape.setPosition(gridX * tileSize, gridY * tileSize);
            tileShape.setTexture(tileTextures[selectedTile]);

            tiles.push_back(tileShape);
        }
    }

    // If right mouse button is pressed, remove the tile
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        if (gridX >= 0 && gridY >= 0 && static_cast<size_t>(gridX) < levelGrid.size() && static_cast<size_t>(gridY) < levelGrid[0].size()) {
            // Find the tile in the tiles vector and remove it
            auto it = std::remove_if(tiles.begin(), tiles.end(), [&](sf::RectangleShape& tile) {
                return tile.getPosition().x == gridX * tileSize && tile.getPosition().y == gridY * tileSize;
            });

            if (it != tiles.end()) {
                tiles.erase(it, tiles.end());
            }

            // Clear the tile from the grid
            levelGrid[gridX][gridY] = TileType::None;
        }
    }
}

void TileManager::saveLevel(const std::string& filename) {
    std::ofstream levelFile(filename);
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
}

void TileManager::loadLevel(const std::string& filename) {
    std::ifstream levelFile(filename);
    if (!levelFile) {
        std::cerr << "Error loading level!" << std::endl;
        return;
    }

    int tileType;
    tiles.clear(); // Clear existing tiles

    for (size_t i = 0; i < levelGrid.size(); ++i) {
        for (size_t j = 0; j < levelGrid[i].size(); ++j) {
            levelFile >> tileType;
            levelGrid[i][j] = static_cast<TileType>(tileType);

            if (levelGrid[i][j] != TileType::None) {
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

    for (unsigned int x = 0; x < window.getSize().x; x += tileSize) {
        line.setSize(sf::Vector2f(1, window.getSize().y));
        line.setPosition(static_cast<float>(x), 0);
        window.draw(line);
    }

    for (unsigned int y = 0; y < window.getSize().y; y += tileSize) {
        line.setSize(sf::Vector2f(window.getSize().x, 1));
        line.setPosition(0, static_cast<float>(y));
        window.draw(line);
    }
}

const std::vector<sf::RectangleShape>& TileManager::getPlacedTiles() const {
    return tiles;  // Return the placed tiles for use in play mode
}

