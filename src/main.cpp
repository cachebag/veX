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
    sf::RenderWindow window(desktopMode, "veX", sf::Style::Default);
    sf::Vector2u windowSize = window.getSize();

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

    for (const auto& tile : placedTiles) {
        const sf::Texture& texture = tileTextures.at(tile.type);
        float x = tile.shape.getPosition().x;
        float y = tile.shape.getPosition().y;
        float width = tile.shape.getSize().x;
        float height = tile.shape.getSize().y;

        platforms.emplace_back(x, y, width, height, texture);
    }

    std::unique_ptr<Player> player;
    if (!platforms.empty()) {
        float platformHeight = platforms.front().getTiles().front().getGlobalBounds().top;
        player = std::make_unique<Player>(500, platformHeight - 100);
    } else {
        player = std::make_unique<Player>(0, 50);
    }

    std::unique_ptr<Enemy> enemy;
    if (!platforms.empty()) {
        float platformHeight = platforms.front().getTiles().front().getGlobalBounds().top;
        enemy = std::make_unique<Enemy>(700, platformHeight - 100);
    } else {
        enemy = std::make_unique<Enemy>(0, 50);
    }

    sf::Text helpText = createHelpText(font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        window.clear();

        float deltaTime = clock.restart().asSeconds();

        float playerX = player->getGlobalBounds().left;

        background.render(window, window.getSize(), playerX, deltaTime);

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

