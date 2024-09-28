#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>  // For std::unique_ptr
#include "../include/TileManager.hpp"
#include "../include/Player.hpp"
#include "../include/Enemy.hpp"
#include "../include/Platform.hpp"
#include "../include/Background.hpp"

enum class GameMode { Play };

// Function to create help text for gameplay
sf::Text createHelpText(const sf::Font& font) {
    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(24);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(20.0f, 20.0f);

    /*helpText.setString(
        "Play Mode\n"
        "WASD to move, Space to jump\n"
        "Press ESC to quit"
    );*/

    return helpText;
}

int main() {
    // Get desktop mode
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "veX", sf::Style::Default);
    sf::Vector2u windowSize = window.getSize();

    // Load font for help text
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", windowSize);

    // Clock for deltaTime
    sf::Clock clock;

    // Create TileManager for loading the level
    TileManager tileManager;

    // Load the initial level from the "levels/level1.txt" file
    if (!tileManager.loadLevelFromFile("levels/level1.txt")) {
        std::cerr << "Error loading level." << std::endl;
        return -1;
    }

    // Platforms for Play Mode
    std::vector<Platform> platforms;

    // Load the placed tiles from the level file
    const auto& placedTiles = tileManager.getPlacedTiles();
    const auto& tileTextures = tileManager.getTileTextures();

    // Convert placed tiles into platforms
    for (const auto& tile : placedTiles) {
        const sf::Texture& texture = tileTextures.at(tile.type);
        float x = tile.shape.getPosition().x;
        float y = tile.shape.getPosition().y;
        float width = tile.shape.getSize().x;
        float height = tile.shape.getSize().y;

        // Create platform from tile
        platforms.emplace_back(x, y, width, height, texture);
    }

    // Create player using std::unique_ptr and place it at the start position
    std::unique_ptr<Player> player;
    if (!platforms.empty()) {
        float platformHeight = platforms.front().getTiles().front().getGlobalBounds().top;
        player = std::make_unique<Player>(500, platformHeight - 100);  // Adjust position as needed
    } else {
        player = std::make_unique<Player>(0, 50);  // Default position if no platforms found
    }

    // Create enemy using std::unique_ptr and place it at a desired position
    std::unique_ptr<Enemy> enemy;
    if (!platforms.empty()) {
        float platformHeight = platforms.front().getTiles().front().getGlobalBounds().top;
        enemy = std::make_unique<Enemy>(700, platformHeight - 100);  // Adjust position as needed
    } else {
        enemy = std::make_unique<Enemy>(0, 50);  // Default position if no platforms found
    }

    // Help text to show controls
    sf::Text helpText = createHelpText(font);

    // Main game loop
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

        // Get player X position for background parallax
        float playerX = player->getGlobalBounds().left;

        background.render(window, window.getSize(), playerX, deltaTime);

        // **Play Mode Logic**

        // Update player movement and logic
        player->update(deltaTime, platforms, windowSize.x, windowSize.y);

        // Update enemy movement and logic
        enemy->update(deltaTime, platforms, windowSize.x, windowSize.y);

        // Draw the platforms
        for (auto& platform : platforms) {
            platform.draw(window);
        }

        // Draw the player
        player->draw(window);

        // Draw the enemy
        enemy->draw(window);

        // Draw help text (controls)
        window.draw(helpText);

        window.display();
    }

    return 0;
}

