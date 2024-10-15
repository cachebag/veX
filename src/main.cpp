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

void drawGrid(sf::RenderWindow& window, const sf::Vector2u& windowSize, float gridSize) {
    sf::RectangleShape line(sf::Vector2f(windowSize.x, 1.0f));
    line.setFillColor(sf::Color(255, 255, 255, 100));

    for (float y = 0; y < windowSize.y; y += gridSize) {
        line.setPosition(0, y);
        window.draw(line);
    }

    line.setSize(sf::Vector2f(1.0f, windowSize.y));

    for (float x = 0; x < windowSize.x; x += gridSize) {
        line.setPosition(x, 0);
        window.draw(line);
    }
}

void saveLevel(const std::vector<sf::Vector2f>& tilePositions) {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_SaveDialog("txt", nullptr, &outPath);
    if (result == NFD_OKAY) {
        std::ofstream outFile(outPath);
        for (const auto& tilePos : tilePositions) {
            outFile << tilePos.x << " " << tilePos.y << "\n";
        }
        outFile.close();
    } else if (result == NFD_CANCEL) {
        std::cerr << "Save cancelled." << std::endl;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

std::vector<sf::Vector2f> loadLevel(std::vector<Platform>& platforms, const sf::Texture& tileTexture, float gridSize) {
    nfdchar_t* outPath = nullptr;
    std::vector<sf::Vector2f> tiles;
    nfdresult_t result = NFD_OpenDialog("txt", nullptr, &outPath);
    if (result == NFD_OKAY) {
        std::ifstream inFile(outPath);
        float x, y;
        platforms.clear();
        while (inFile >> x >> y) {
            sf::Vector2f pos(x, y);
            tiles.push_back(pos);
            platforms.emplace_back(x, y, gridSize, gridSize, tileTexture); 
        }
        inFile.close();
    } else if (result == NFD_CANCEL) {
        std::cerr << "Load cancelled." << std::endl;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
    return tiles;
}

int main() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "veX", sf::Style::Default);
    sf::Vector2u windowSize = window.getSize();
    float baseWidth = 1920.0f;
    float baseHeight = 1080.0f;
    float scaleX = static_cast<float>(windowSize.x) / baseWidth;
    float scaleY = static_cast<float>(windowSize.y) / baseHeight;

    float scaleFactor = std::min(scaleX, scaleY);

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", windowSize);

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

    //tilePositions = loadLevel(platforms, tileTexture, gridSize); // This allows us to select a level upon running the game 

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                sf::Vector2u windowSize = window.getSize();
                float scaleX = static_cast<float>(windowSize.x) / baseWidth;
                float scaleY = static_cast<float>(windowSize.y) / baseHeight;
                float scaleFactor = std::min(scaleX, scaleY);

                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));

      }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::E) {
                    currentMode = (currentMode == GameMode::Play) ? GameMode::Edit : GameMode::Play;
                }
                if (event.key.code == sf::Keyboard::D && currentMode == GameMode::Edit) {
                    debugMode = !debugMode;
                }
                if (event.key.code == sf::Keyboard::S) {
                    saveLevel(tilePositions);
                }
                if (event.key.code == sf::Keyboard::L) {
                    tilePositions = loadLevel(platforms, tileTexture, gridSize);
                }
            }

            if (currentMode == GameMode::Edit && loadLevel) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::E) {
                        player = std::make_unique<Player>(0, 0); // i think this stops the player from spawning in the tiles when loading a level? lol     
          }
        }
      }

            if (currentMode == GameMode::Edit) {
              sf::Vector2i mousePos = sf::Mouse::getPosition(window);
              sf::Vector2f tilePos(static_cast<float>(mousePos.x) - (mousePos.x % static_cast<int>(gridSize)),
                                  static_cast<float>(mousePos.y) - (mousePos.y % static_cast<int>(gridSize)));
              if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                  if (std::find(tilePositions.begin(), tilePositions.end(), tilePos) == tilePositions.end()) {
                      tilePositions.push_back(tilePos);
                      platforms.emplace_back(tilePos.x, tilePos.y, gridSize, gridSize, tileTexture);  // Add platform
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
        background.render(window, window.getSize(), playerX, deltaTime);

        for (const auto& pos : tilePositions) {
            sf::Sprite tile(tileTexture);
            tile.setPosition(pos);
            window.draw(tile);
        }

        if (currentMode == GameMode::Play) {
            player->update(deltaTime, platforms, windowSize.x, windowSize.y);
            enemy->update(deltaTime, platforms, windowSize.x, windowSize.y);
            player->draw(window);
            enemy->draw(window);
        }

        if (currentMode == GameMode::Edit && debugMode) {
            drawGrid(window, windowSize, gridSize);
        }

        window.display();
    }

    return 0;
}

