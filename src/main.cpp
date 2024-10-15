#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "../include/nfd.h"
#include "../include/Player.hpp"
#include "../include/Enemy.hpp"
#include "../include/Background.hpp"
#include "../include/Platform.hpp"

enum class GameMode { Play, Edit };

void drawGrid(sf::RenderWindow& window, const sf::Vector2f& viewSize, float gridSize) {
    sf::RectangleShape line(sf::Vector2f(viewSize.x, 1.0f));
    line.setFillColor(sf::Color(255, 255, 255, 100));

    for (float y = 0; y < viewSize.y; y += gridSize) {
        line.setPosition(0, y);
        window.draw(line);
    }

    line.setSize(sf::Vector2f(1.0f, viewSize.y));

    for (float x = 0; x < viewSize.x; x += gridSize) {
        line.setPosition(x, 0);
        window.draw(line);
    }
}

void saveLevel(sf::RenderWindow& window, const std::vector<sf::Vector2f>& tilePositions) {
    // Switch to windowed mode temporarily
    window.create(sf::VideoMode(1280, 720), "veX - Saving...", sf::Style::Close);

    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_SaveDialog("txt", nullptr, &outPath);
    if (result == NFD_OKAY) {
        std::ofstream outFile(outPath);
        if (!outFile) {
            std::cerr << "Error: Could not open file for saving." << std::endl;
            return;
        }

        for (const auto& tilePos : tilePositions) {
            outFile << tilePos.x << " " << tilePos.y << "\n";  // Save each tile position
        }

        outFile.close();
        std::cout << "Level saved successfully." << std::endl;
    } else if (result == NFD_CANCEL) {
        std::cerr << "Save cancelled." << std::endl;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }

    // Restore fullscreen mode
    window.create(sf::VideoMode::getDesktopMode(), "veX", sf::Style::Fullscreen);
}

std::vector<sf::Vector2f> loadLevel(sf::RenderWindow& window, std::vector<Platform>& platforms, const sf::Texture& tileTexture, float gridSize) {
    // Switch to windowed mode temporarily
    window.create(sf::VideoMode(1280, 720), "veX - Loading...", sf::Style::Close);

    nfdchar_t* outPath = nullptr;
    std::vector<sf::Vector2f> tiles;
    nfdresult_t result = NFD_OpenDialog("txt", nullptr, &outPath);
    if (result == NFD_OKAY) {
        std::ifstream inFile(outPath);
        if (!inFile) {
            std::cerr << "Error: Could not open file for loading." << std::endl;
            return tiles;
        }

        float x, y;
        platforms.clear();  // Clear existing platforms
        tiles.clear();      // Clear existing tiles
        while (inFile >> x >> y) {
            sf::Vector2f pos(x, y);
            tiles.push_back(pos);  // Add the tile position
            platforms.emplace_back(pos.x, pos.y, gridSize, gridSize, tileTexture);  // Add platform
        }

        inFile.close();
        std::cout << "Level loaded successfully." << std::endl;
    } else if (result == NFD_CANCEL) {
        std::cerr << "Load cancelled." << std::endl;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }

    // Restore fullscreen mode
    window.create(sf::VideoMode::getDesktopMode(), "veX", sf::Style::Fullscreen);

    return tiles;
}

void updateView(sf::RenderWindow& window, sf::View& view, const sf::Vector2u& baseResolution) {
    sf::Vector2u windowSize = window.getSize();
    float aspectRatio = float(windowSize.x) / float(windowSize.y);
    float baseAspectRatio = float(baseResolution.x) / float(baseResolution.y);

    if (aspectRatio > baseAspectRatio) {
        // Adjust width to maintain aspect ratio
        float newWidth = baseResolution.y * aspectRatio;
        view.setSize(newWidth, baseResolution.y);
    } else {
        // Adjust height to maintain aspect ratio
        float newHeight = baseResolution.x / aspectRatio;
        view.setSize(baseResolution.x, newHeight);
    }
    window.setView(view);
}

int main() {
    // Base resolution that everything is designed around
    sf::Vector2u baseResolution(1920, 1080);

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "veX", sf::Style::Fullscreen);
    sf::View view(sf::FloatRect(0, 0, baseResolution.x, baseResolution.y));
    window.setView(view);

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", baseResolution);

    sf::Texture tileTexture;
    if (!tileTexture.loadFromFile("assets/tutorial_level/brick.png")) {
        std::cerr << "Error loading tile texture." << std::endl;
        return -1;
    }

    sf::Clock clock;
    std::unique_ptr<Player> player = std::make_unique<Player>(0, 0);
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(2400, 1100);
    GameMode currentMode = GameMode::Play;
    std::vector<sf::Vector2f> tilePositions;
    bool debugMode = false;
    const float gridSize = 64.0f;
    std::vector<Platform> platforms;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                updateView(window, view, baseResolution);
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::E) {
                    currentMode = (currentMode == GameMode::Play) ? GameMode::Edit : GameMode::Play;
                }
                if (event.key.code == sf::Keyboard::D && currentMode == GameMode::Edit) {
                    debugMode = !debugMode;
                }
                if (event.key.code == sf::Keyboard::S) {
                    saveLevel(window, tilePositions);
                }
                if (event.key.code == sf::Keyboard::L) {
                    tilePositions = loadLevel(window, platforms, tileTexture, gridSize);
                }
            }

            if (currentMode == GameMode::Edit) {
                // Get the mouse position in the window (pixels) and convert it to world coordinates (game coordinates)
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                // Align the tile position to the grid by snapping the world position to the nearest grid cell
                sf::Vector2f tilePos(static_cast<float>(static_cast<int>(worldPos.x / gridSize) * gridSize),
                                     static_cast<float>(static_cast<int>(worldPos.y / gridSize) * gridSize));

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    // Only place a tile if it's not already placed at the same position
                    if (std::find(tilePositions.begin(), tilePositions.end(), tilePos) == tilePositions.end()) {
                        tilePositions.push_back(tilePos);
                        platforms.emplace_back(tilePos.x, tilePos.y, gridSize, gridSize, tileTexture);  // Add platform
                    }
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    // Find and remove the tile if it exists at the current position
                    auto it = std::find(tilePositions.begin(), tilePositions.end(), tilePos);
                    if (it != tilePositions.end()) {
                        int index = std::distance(tilePositions.begin(), it);
                        tilePositions.erase(it);
                        platforms.erase(platforms.begin() + index);
                    }
                }
            }
        }

        window.clear();
        float playerX = player->getGlobalBounds().left;
        float deltaTime = clock.restart().asSeconds();
        background.render(window, baseResolution, playerX, deltaTime);

        for (const auto& pos : tilePositions) {
            sf::Sprite tile(tileTexture);
            tile.setPosition(pos);
            window.draw(tile);
        }

        if (currentMode == GameMode::Play) {
            player->update(deltaTime, platforms, window.getSize().x, window.getSize().y);
            enemy->update(deltaTime, platforms, window.getSize().x, window.getSize().y);
            player->draw(window);
            enemy->draw(window);
        }

        if (currentMode == GameMode::Edit && debugMode) {
            drawGrid(window, view.getSize(), gridSize);
        }

        window.display();
    }

    return 0;
}

