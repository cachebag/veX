#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include "../include/TileManager.hpp"
#include "../include/Player.hpp"
#include "../include/Enemy.hpp"
#include "../include/Platform.hpp"
#include "../include/Background.hpp"

enum class GameMode { Play };

sf::Text createHelpText(const sf::Font& font) {
    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(24);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(20.0f, 20.0f);
    return helpText;
}

int main() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "veX", sf::Style::Fullscreen);
    sf::Vector2u windowSize = window.getSize();

    // Adjust the view for scaling
    sf::View view(sf::FloatRect(0, 0, static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    window.setView(view);

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", windowSize);

    sf::Clock clock;

    TileManager tileManager;

    if (!tileManager.loadLevelFromFile("levels/level1.txt")) {
        std::cerr << "Error loading level." << std::endl;
        return -1;
    }

    std::vector<Platform> platforms;

    const auto& placedTiles = tileManager.getPlacedTiles();
    const auto& tileTextures = tileManager.getTileTextures();

    // Scale platform tiles relative to window size
    float scaleFactorX = static_cast<float>(windowSize.x) / 1920.0f;  // Example: 1920 is reference width
    float scaleFactorY = static_cast<float>(windowSize.y) / 1080.0f;  // Example: 1080 is reference height

    for (const auto& tile : placedTiles) {
        const sf::Texture& texture = tileTextures.at(tile.type);
        float x = tile.shape.getPosition().x * scaleFactorX;
        float y = tile.shape.getPosition().y * scaleFactorY;
        float width = tile.shape.getSize().x * scaleFactorX;
        float height = tile.shape.getSize().y * scaleFactorY;

        platforms.emplace_back(x, y, width, height, texture);
    }

    std::unique_ptr<Player> player;
    if (!platforms.empty()) {
        float platformHeight = platforms.front().getTiles().front().getGlobalBounds().top;
        player = std::make_unique<Player>(500 * scaleFactorX, platformHeight - 100);
    } else {
        player = std::make_unique<Player>(0 * scaleFactorX, 50 * scaleFactorY);
    }

    // Position the enemy in the center of the screen
    float enemyStartX = windowSize.x / 2.0f;  // Centered horizontally
    float enemyStartY = windowSize.y / 2.0f - 100;  // Adjust Y to float a bit above the ground level
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(enemyStartX, enemyStartY);

    sf::Text helpText = createHelpText(font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            // Handle resizing
            if (event.type == sf::Event::Resized) {
                windowSize = window.getSize();
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                view = sf::View(visibleArea);
                window.setView(view);

                // Recalculate scale factors based on new window size
                scaleFactorX = static_cast<float>(windowSize.x) / 1920.0f;
                scaleFactorY = static_cast<float>(windowSize.y) / 1080.0f;

                // Rescale the platforms
                for (auto& platform : platforms) {
                    platform.rescale(scaleFactorX, scaleFactorY);
                }
            }
        }

        window.clear();

        float deltaTime = clock.restart().asSeconds();

        float playerX = player->getGlobalBounds().left;

        background.render(window, windowSize, playerX, deltaTime);

        player->update(deltaTime, platforms, windowSize.x, windowSize.y);

        enemy->update(deltaTime, platforms, windowSize.x, windowSize.y);

        for (auto& platform : platforms) {
            platform.draw(window);
        }

        player->draw(window);

        enemy->draw(window);

        window.draw(helpText);

        window.display();
    }

    return 0;
}

