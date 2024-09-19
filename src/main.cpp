#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>  // For std::unique_ptr
#include "../include/TileManager.hpp"
#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include "../include/Background.hpp"

enum class GameMode { Play, Edit };

// Function to create help text based on current mode
sf::Text createHelpText(const sf::Font& font, GameMode mode) {
    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(24);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(20.0f, 20.0f);

    if (mode == GameMode::Play) {
        helpText.setString(
            "Play Mode\n"
            "WASD to move, Space to jump\n"
            "Press E to switch to Edit Mode\n"
            "Press ESC to quit"
        );
    } else if (mode == GameMode::Edit) {
        helpText.setString(
            "Edit Mode\n"
            "Left Click to place tile\n"
            "Press S to save | Press L to load\n"
            "Press P to switch to Play Mode\n"
            "Press D to toggle debug (grid)\n"
            "Press ESC to quit"
        );
    }

    return helpText;
}

int main() {
    // Get desktop mode
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "veX", sf::Style::Default);
    sf::Vector2u windowSize = window.getSize();

    // Load font for orb counter and help text
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

    // Create TileManager for the level editor
    TileManager tileManager;

    // Create player using std::unique_ptr
    std::unique_ptr<Player> player = std::make_unique<Player>(0, 0);  // Initial position of player

    // Platforms for Play Mode
    std::vector<Platform> platforms;

    // Set initial mode to Play Mode
    GameMode currentMode = GameMode::Play;

    // Help text to show controls
    sf::Text helpText = createHelpText(font, currentMode);

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            // Mode switching logic
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::E) {
                    currentMode = GameMode::Edit;  // Switch to editor mode
                    helpText = createHelpText(font, currentMode);  // Update help text
                } else if (event.key.code == sf::Keyboard::P) {
                    currentMode = GameMode::Play;  // Switch to play mode

                    // Load the placed tiles from the level editor
                    const auto& placedTiles = tileManager.getPlacedTiles();
                    const auto& tileTextures = tileManager.getTileTextures();

                    platforms.clear();  // Clear existing platforms

                    // Convert placed tiles into platforms
                    for (const auto& tile : placedTiles) {
                        const sf::Texture& texture = tileTextures.at(tile.type);
                        float x = tile.shape.getPosition().x;
                        float y = tile.shape.getPosition().y;
                        float width = tile.shape.getSize().x;
                        float height = tile.shape.getSize().y;

                        // Create platform from tile
                        platforms.emplace_back(x, y, width, height, texture, windowSize);
                    }

                    // Reset the player to start position (e.g., top of first platform)
                    if (!platforms.empty()) {
                        float platformHeight = platforms.front().getTiles().front().getGlobalBounds().top;
                        player = std::make_unique<Player>(0, platformHeight - player->getGlobalBounds().height);
                    }

                    helpText = createHelpText(font, currentMode);  // Update help text
                }

                // Handle saving and loading in Edit mode
                if (currentMode == GameMode::Edit) {
                    if (event.key.code == sf::Keyboard::S) {
                        tileManager.saveLevel();
                    } else if (event.key.code == sf::Keyboard::L) {
                        tileManager.loadLevel();
                    }
                }
            }

            // Debug mode toggle (for grid lines, etc.)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                tileManager.toggleDebugMode();
            }

            // Handle tile placement in Edit mode
            if (currentMode == GameMode::Edit) {
                tileManager.handleInput(window);
            }
        }

        window.clear();

        float playerX = player->getGlobalBounds().left;

        float deltaTime = clock.restart().asSeconds();

        background.render(window, window.getSize(), playerX, deltaTime);

        // Handle the two game modes
        if (currentMode == GameMode::Play) {
            // **Play Mode Logic**

            // Update player movement and logic
            player->update(deltaTime, platforms, windowSize.x, windowSize.y);

            // Draw the platforms
            for (auto& platform : platforms) {
                platform.draw(window);
            }

            // Draw the player
            player->draw(window);

        } else if (currentMode == GameMode::Edit) {
            // **Editor Mode Logic**
            clock.restart();  // Reset clock to avoid sudden jumps in play mode

            // Draw the tiles in the editor
            tileManager.draw(window);

            // Draw a preview of the currently selected tile
            tileManager.drawTilePreview(window);

            // Draw the sidebar with tile selection and save/load buttons
            tileManager.drawSidebar(window);
        }

        // Draw help text (controls)
        window.draw(helpText);

        window.display();
    }

    return 0;
}

