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

// Function to adjust the view and maintain aspect ratio
void updateView(sf::RenderWindow& window, sf::View& view, const sf::Vector2u& internalResolution) {
    sf::Vector2u windowSize = window.getSize();

    float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;
    float internalAspectRatio = static_cast<float>(internalResolution.x) / internalResolution.y;

    if (windowAspectRatio > internalAspectRatio) {
        // Add pillarboxing (black bars on the sides)
        float viewportWidth = internalAspectRatio / windowAspectRatio;
        view.setViewport(sf::FloatRect((1.0f - viewportWidth) / 2.0f, 0.0f, viewportWidth, 1.0f));
    } else {
        // Add letterboxing (black bars on the top and bottom)
        float viewportHeight = windowAspectRatio / internalAspectRatio;
        view.setViewport(sf::FloatRect(0.0f, (1.0f - viewportHeight) / 2.0f, 1.0f, viewportHeight));
    }

    // Center the view and apply it to the window
    view.setSize(internalResolution.x, internalResolution.y);
    view.setCenter(internalResolution.x / 2.0f, internalResolution.y / 2.0f);
    window.setView(view);
}

int main() {
    // Set internal resolution for the game logic
    sf::Vector2u internalResolution(1920, 1080);  // Fixed internal resolution

    sf::RenderWindow window;

    #ifdef __APPLE__
        // On macOS, use windowed fullscreen (borderless) to prevent segfaults
        window.create(sf::VideoMode::getDesktopMode(), "veX", sf::Style::None);
    #else
        // For other platforms (Windows/Linux), use regular fullscreen mode
        window.create(sf::VideoMode::getDesktopMode(), "veX", sf::Style::Fullscreen);
    #endif

    sf::View view(sf::FloatRect(0, 0, internalResolution.x, internalResolution.y));
    window.setView(view);

    // Apply the view scaling at startup
    updateView(window, view, internalResolution);

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", internalResolution);

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
                updateView(window, view, internalResolution);  // Adjust view on resize
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::E) {
                    currentMode = (currentMode == GameMode::Play) ? GameMode::Edit : GameMode::Play;
                }
                if (event.key.code == sf::Keyboard::D && currentMode == GameMode::Edit) {
                    debugMode = !debugMode;
                }
                if (event.key.code == sf::Keyboard::S) {
                    // Implement saveLevel here
                }
                if (event.key.code == sf::Keyboard::L) {
                    // Implement loadLevel here
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
        background.render(window, internalResolution, playerX, deltaTime);

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

