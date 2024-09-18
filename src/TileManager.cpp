#include "../include/TileManager.hpp"
#include <fstream>
#include <iostream>
#include "../include/nfd.h" // Include native file dialog header

TileManager::TileManager() {
    loadTextures();
    levelGrid.resize(100, std::vector<TileType>(100, TileType::None)); // 100x100 grid for tile placement
}

void TileManager::loadTextures() {
    // Load each texture from the assets folder and associate it with a TileType
    if (!tileTextures[TileType::Brick].loadFromFile("assets/tutorial_level/brick.png"))
        std::cerr << "Error loading brick texture!" << std::endl;

    if (!tileTextures[TileType::BushLarge].loadFromFile("assets/tutorial_level/bush-large.png"))
        std::cerr << "Error loading bush-large texture!" << std::endl;

    if (!tileTextures[TileType::BushSmall].loadFromFile("assets/tutorial_level/bush-small.png"))
        std::cerr << "Error loading bush-small texture!" << std::endl;

    if (!tileTextures[TileType::SlimeBlock].loadFromFile("assets/tutorial_level/slime-block.png"))
        std::cerr << "Error loading slime-block texture!" << std::endl;

    if (!tileTextures[TileType::Statue].loadFromFile("assets/tutorial_level/statue.png"))
        std::cerr << "Error loading statue texture!" << std::endl;

    if (!tileTextures[TileType::Stone1].loadFromFile("assets/tutorial_level/stone-1.png"))
        std::cerr << "Error loading stone-1 texture!" << std::endl;

    if (!tileTextures[TileType::Stone2].loadFromFile("assets/tutorial_level/stone-2.png"))
        std::cerr << "Error loading stone-2 texture!" << std::endl;

    if (!tileTextures[TileType::Stone3].loadFromFile("assets/tutorial_level/stone-3.png"))
        std::cerr << "Error loading stone-3 texture!" << std::endl;

    if (!tileTextures[TileType::Stone4].loadFromFile("assets/tutorial_level/stone-4.png"))
        std::cerr << "Error loading stone-4 texture!" << std::endl;

    if (!tileTextures[TileType::Tree1].loadFromFile("assets/tutorial_level/tree-1.png"))
        std::cerr << "Error loading tree-1 texture!" << std::endl;

    if (!tileTextures[TileType::Tree2].loadFromFile("assets/tutorial_level/tree-2.png"))
        std::cerr << "Error loading tree-2 texture!" << std::endl;

    if (!tileTextures[TileType::Tree3].loadFromFile("assets/tutorial_level/tree-3.png"))
        std::cerr << "Error loading tree-3 texture!" << std::endl;

    if (!tileTextures[TileType::Rock].loadFromFile("assets/tutorial_level/rock.png"))
        std::cerr << "Error loading rock texture!" << std::endl;
}

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
                    sf::RectangleShape tileShape(sf::Vector2f(tileDrawSize, tileDrawSize));
                    tileShape.setPosition(i * tileDrawSize, j * tileDrawSize);
                    tileShape.setTexture(&tileTextures[levelGrid[i][j]]);

                    tiles.push_back(tileShape);
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

void TileManager::toggleDebugMode() {
    debugMode = !debugMode;
}

void TileManager::drawSidebar(sf::RenderWindow& window) {
    sf::RectangleShape sidebar(sf::Vector2f(200, window.getSize().y));
    sidebar.setFillColor(sf::Color(50, 50, 50));
    sidebar.setPosition(window.getSize().x - 200, 0);
    window.draw(sidebar);

    float x = window.getSize().x - 180;
    float y = 20.0f;

    for (const auto& [tileType, texture] : tileTextures) {
        sf::RectangleShape tileButton(sf::Vector2f(tileSize, tileSize));
        tileButton.setPosition(x, y);
        tileButton.setTexture(&texture);
        tileButton.setOutlineThickness(2);
        tileButton.setOutlineColor(tileType == selectedTile ? sf::Color::Red : sf::Color::Transparent);
        window.draw(tileButton);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (tileButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                selectedTile = tileType;  // Select the tile
            }
        }

        y += tileSize + 10.0f;  // Move down for the next tile
    }

    sf::RectangleShape saveButton(sf::Vector2f(150, 50));
    saveButton.setPosition(window.getSize().x - 180, window.getSize().y - 150);
    saveButton.setFillColor(sf::Color::Blue);
    saveButton.setOutlineThickness(2);
    saveButton.setOutlineColor(sf::Color::White);

    sf::RectangleShape loadButton(sf::Vector2f(150, 50));
    loadButton.setPosition(window.getSize().x - 180, window.getSize().y - 80);
    loadButton.setFillColor(sf::Color::Green);
    loadButton.setOutlineThickness(2);
    loadButton.setOutlineColor(sf::Color::White);

    window.draw(saveButton);
    window.draw(loadButton);

    sf::Font font;
    font.loadFromFile("assets/fonts/Merriweather-Regular.ttf");
    sf::Text saveText("Save", font, 20);
    saveText.setPosition(window.getSize().x - 140, window.getSize().y - 140);
    window.draw(saveText);

    sf::Text loadText("Load", font, 20);
    loadText.setPosition(window.getSize().x - 140, window.getSize().y - 70);
    window.draw(loadText);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (saveButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            saveLevel();
        }
        if (loadButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            loadLevel();
        }
    }
}

void TileManager::handleInput(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    int gridX = mousePos.x / tileDrawSize;
    int gridY = mousePos.y / tileDrawSize;

    if (isMouseInSidebar(window)) {
        return;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (gridX >= 0 && gridY >= 0 && static_cast<size_t>(gridX) < levelGrid.size() && static_cast<size_t>(gridY) < levelGrid[0].size()) {
            levelGrid[gridX][gridY] = selectedTile;

            sf::RectangleShape tileShape(sf::Vector2f(tileDrawSize, tileDrawSize));
            tileShape.setPosition(gridX * tileDrawSize, gridY * tileDrawSize);
            tileShape.setTexture(&tileTextures[selectedTile]);

            tiles.push_back(tileShape);
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        if (gridX >= 0 && gridY >= 0 && static_cast<size_t>(gridX) < levelGrid.size() && static_cast<size_t>(gridY) < levelGrid[0].size()) {
            auto it = std::remove_if(tiles.begin(), tiles.end(), [&](sf::RectangleShape& tile) {
                return tile.getPosition().x == gridX * tileDrawSize && tile.getPosition().y == gridY * tileDrawSize;
            });

            if (it != tiles.end()) {
                tiles.erase(it, tiles.end());
            }

            levelGrid[gridX][gridY] = TileType::None;
        }
    }
}

void TileManager::drawTilePreview(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::RectangleShape preview(sf::Vector2f(tileDrawSize, tileDrawSize));
    preview.setPosition(static_cast<float>(mousePos.x - mousePos.x % tileDrawSize), static_cast<float>(mousePos.y - mousePos.y % tileDrawSize));
    preview.setTexture(&tileTextures[selectedTile]);
    window.draw(preview);
}

bool TileManager::isMouseInSidebar(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    return mousePos.x >= window.getSize().x - 200;
}

void TileManager::draw(sf::RenderWindow& window) {
    for (const auto& tile : tiles) {
        window.draw(tile);
    }

    if (debugMode) {
        drawGrid(window);
    }
}

void TileManager::drawGrid(sf::RenderWindow& window) {
    sf::RectangleShape line;
    line.setFillColor(sf::Color::Green);

    for (unsigned int x = 0; x < window.getSize().x; x += tileDrawSize) {
        line.setSize(sf::Vector2f(1, window.getSize().y));
        line.setPosition(static_cast<float>(x), 0);
        window.draw(line);
    }

    for (unsigned int y = 0; y < window.getSize().y; y += tileDrawSize) {
        line.setSize(sf::Vector2f(window.getSize().x, 1));
        line.setPosition(0, static_cast<float>(y));
        window.draw(line);
    }
}

const std::vector<sf::RectangleShape>& TileManager::getPlacedTiles() const {
    return tiles;
}

