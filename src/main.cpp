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

    #ifdef __APPLE__
        window.create(sf::VideoMode(1920, 1080, 32), "veX", sf::Style::None);
    #else
        window.create(sf::VideoMode::getDesktopMode(), "veX", sf::Style::Fullscreen);
    #endif
}

std::vector<sf::Vector2f> loadLevelFromFile(const std::string& filepath, std::vector<Platform>& platforms, const sf::Texture& tileTexture, float gridSize) {
    std::vector<sf::Vector2f> tiles;
    std::ifstream inFile(filepath);
    float x, y;
    platforms.clear();
    while (inFile >> x >> y) {
        sf::Vector2f pos(x, y);
        tiles.push_back(pos);
        platforms.emplace_back(x, y, gridSize, gridSize, tileTexture);
    }
    inFile.close();
    return tiles;
}

std::vector<sf::Vector2f> loadLevel(sf::RenderWindow& window, std::vector<Platform>& platforms, const sf::Texture& tileTexture, float gridSize) {
    window.create(sf::VideoMode(1280, 720), "veX - Loading...", sf::Style::Close);

    std::vector<sf::Vector2f> tiles;
    nfdchar_t* outPath = nullptr;
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
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }

    #ifdef __APPLE__
        window.create(sf::VideoMode(1920, 1080), "veX", sf::Style::None);
    #else
        window.create(sf::VideoMode(1920, 1080), "veX", sf::Style::None);
    #endif

    return tiles;
}

void updateView(sf::RenderWindow& window, sf::View& view) {
    sf::Vector2u windowSize = window.getSize();

    float baseWidth = 1920.0f;
    float baseHeight = 1080.0f;
    
    float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;
    float internalAspectRatio = baseWidth / baseHeight;

    if (windowAspectRatio > internalAspectRatio) {
        float viewportWidth = internalAspectRatio / windowAspectRatio;
        view.setViewport(sf::FloatRect((1.0f - viewportWidth) / 2.0f, 0.0f, viewportWidth, 1.0f));
    } else {
        float viewportHeight = windowAspectRatio / internalAspectRatio;
        view.setViewport(sf::FloatRect(0.0f, (1.0f - viewportHeight) / 2.0f, 1.0f, viewportHeight));
    }

    view.setSize(baseWidth, baseHeight);
    view.setCenter(baseWidth / 2.0f, baseHeight / 2.0f);
    window.setView(view);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1200), "veX", sf::Style::Fullscreen);  

    sf::View view;
    updateView(window, view);

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
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(1700, 800);
    GameMode currentMode = GameMode::Play;
    bool debugMode = false;
    const float gridSize = 64.0f;
    std::vector<Platform> platforms;

    std::vector<sf::Vector2f> tilePositions = loadLevelFromFile("levels/level1.txt", platforms, tileTexture, gridSize);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                updateView(window, view);
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
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                sf::Vector2f tilePos(static_cast<float>(static_cast<int>(worldPos.x / gridSize) * gridSize),
                                     static_cast<float>(static_cast<int>(worldPos.y / gridSize) * gridSize));

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (std::find(tilePositions.begin(), tilePositions.end(), tilePos) == tilePositions.end()) {
                        tilePositions.push_back(tilePos);
                        platforms.emplace_back(tilePos.x, tilePos.y, gridSize, gridSize, tileTexture);
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
            window.draw(tile);
        }

        if (currentMode == GameMode::Play) {
            player->update(deltaTime, platforms, window.getSize().x, window.getSize().y, *enemy);
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
