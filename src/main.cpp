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

float getScalingFactor(sf::RenderWindow& window, const sf::Vector2f& baseResolution) {
    sf::Vector2u currentSize = window.getSize();
    float scaleX = static_cast<float>(currentSize.x) / baseResolution.x;
    float scaleY = static_cast<float>(currentSize.y) / baseResolution.y;
    return std::min(scaleX, scaleY);  // Keep uniform scaling
}

void drawGrid(sf::RenderWindow& window, const sf::Vector2f& viewSize, float gridSize, float scaleFactor) {
    sf::RectangleShape line(sf::Vector2f(viewSize.x, 1.0f * scaleFactor));
    line.setFillColor(sf::Color(255, 255, 255, 100));

    for (float y = 0; y < viewSize.y; y += gridSize * scaleFactor) {
        line.setPosition(0, y);
        window.draw(line);
    }

    line.setSize(sf::Vector2f(1.0f * scaleFactor, viewSize.y));

    for (float x = 0; x < viewSize.x; x += gridSize * scaleFactor) {
        line.setPosition(x, 0);
        window.draw(line);
    }
}

void saveLevel(sf::RenderWindow& window, const std::vector<sf::Vector2f>& tilePositions) {
    window.create(sf::VideoMode(1280, 720), "veX - Saving...", sf::Style::Close);

    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_SaveDialog("txt", nullptr, &outPath);
    if (result == NFD_OKAY) {
        std::ofstream outFile(outPath);
        for (const auto& tilePos : tilePositions) {
            outFile << tilePos.x << " " << tilePos.y << "\n";
        }
        outFile.close();
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }

    window.create(sf::VideoMode::getDesktopMode(), "veX", sf::Style::Fullscreen);
}

std::vector<sf::Vector2f> loadLevelFromFile(const std::string& filepath, std::vector<Platform>& platforms, const sf::Texture& tileTexture, float gridSize, float scaleFactor) {
    std::vector<sf::Vector2f> tiles;
    std::ifstream inFile(filepath);
    float x, y;
    platforms.clear();
    while (inFile >> x >> y) {
        sf::Vector2f pos(x * scaleFactor, y * scaleFactor);  // Scale positions
        tiles.push_back(pos);
        platforms.emplace_back(pos.x, pos.y, gridSize * scaleFactor, gridSize * scaleFactor, tileTexture);
    }
    inFile.close();
    return tiles;
}

void updateView(sf::RenderWindow& window, sf::View& view, const sf::Vector2f& baseResolution) {
    sf::Vector2u windowSize = window.getSize();
    float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;
    float baseAspectRatio = baseResolution.x / baseResolution.y;

    if (windowAspectRatio > baseAspectRatio) {
        float viewportWidth = baseAspectRatio / windowAspectRatio;
        view.setViewport(sf::FloatRect((1.0f - viewportWidth) / 2.0f, 0.0f, viewportWidth, 1.0f));
    } else {
        float viewportHeight = windowAspectRatio / baseAspectRatio;
        view.setViewport(sf::FloatRect(0.0f, (1.0f - viewportHeight) / 2.0f, 1.0f, viewportHeight));
    }

    view.setSize(baseResolution);
    view.setCenter(baseResolution.x / 2.0f, baseResolution.y / 2.0f);
    window.setView(view);
}

int main() {
    const sf::Vector2f baseResolution(1920.0f, 1080.0f);

    sf::RenderWindow window(sf::VideoMode(1366, 768), "veX", sf::Style::Fullscreen);
    sf::View view;
    updateView(window, view, baseResolution);

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", sf::Vector2u(1920, 1080));

    sf::Texture tileTexture;
    if (!tileTexture.loadFromFile("assets/tutorial_level/brick.png")) {
        std::cerr << "Error loading tile texture." << std::endl;
        return -1;
    }

    sf::Clock clock;
    std::unique_ptr<Player> player = std::make_unique<Player>(0, 0);
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(1400, 800);
    GameMode currentMode = GameMode::Play;
    bool debugMode = false;
    const float gridSize = 64.0f;
    std::vector<Platform> platforms;

    float scaleFactor = getScalingFactor(window, baseResolution);  // Calculate scaling factor
    std::vector<sf::Vector2f> tilePositions = loadLevelFromFile("levels/level1.txt", platforms, tileTexture, gridSize, scaleFactor);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                updateView(window, view, baseResolution);
                scaleFactor = getScalingFactor(window, baseResolution);  // Update scaling factor on resize
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
                    tilePositions = loadLevelFromFile("levels/level1.txt", platforms, tileTexture, gridSize, scaleFactor);
                }
            }

            if (currentMode == GameMode::Edit) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                sf::Vector2f tilePos(static_cast<float>(static_cast<int>(worldPos.x / (gridSize * scaleFactor)) * (gridSize * scaleFactor)),
                                     static_cast<float>(static_cast<int>(worldPos.y / (gridSize * scaleFactor)) * (gridSize * scaleFactor)));

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (std::find(tilePositions.begin(), tilePositions.end(), tilePos) == tilePositions.end()) {
                        tilePositions.push_back(tilePos);
                        platforms.emplace_back(tilePos.x, tilePos.y, gridSize * scaleFactor, gridSize * scaleFactor, tileTexture);
                    }
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
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
        background.render(window, sf::Vector2u(1920, 1080), playerX, deltaTime);

        for (const auto& pos : tilePositions) {
            sf::Sprite tile(tileTexture);
            tile.setPosition(pos);
            tile.setScale(scaleFactor, scaleFactor);  // Apply scaling to tiles
            window.draw(tile);
        }

        if (currentMode == GameMode::Play) {
            player->update(deltaTime, platforms, window.getSize().x, window.getSize().y, *enemy, scaleFactor);
            enemy->update(deltaTime, platforms, window.getSize().x, window.getSize().y, scaleFactor);
            player->draw(window);
            enemy->draw(window);
        }

        if (currentMode == GameMode::Edit && debugMode) {
            drawGrid(window, view.getSize(), gridSize, scaleFactor);  // Apply scaling to the grid
        }

        window.display();
    }

    return 0;
}

