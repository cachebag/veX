#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "TileManager.hpp"
#include "../include/Player.hpp"
#include "../include/Platform.hpp"

enum class GameMode { Play, Edit };

// Function to create help text based on current mode
sf::Text createHelpText(const sf::Font& font, GameMode mode) {
    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(24);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(20.0f, 20.0f);

    // Display different help messages depending on the current mode
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
            "1: Ground tile | 2: Ground2 tile | 3: Ground3 tile\n"
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
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    // Orb counter text
    sf::Text orbCounterText;
    orbCounterText.setFont(font);
    orbCounterText.setCharacterSize(24);
    orbCounterText.setFillColor(sf::Color::White);
    orbCounterText.setPosition(1700.0f, 20.0f);

    // Load texture (using only ground3.png for now)
    sf::Texture groundTexture;
    if (!groundTexture.loadFromFile("assets/tutorial_level/ground3.png")) {
        std::cerr << "Error loading ground texture!" << std::endl;
        return -1;
    }

    // Create platform and player for Play Mode
    Platform groundPlatform(0.0f, 0.9f, 1.0f, 0.1f, groundTexture, windowSize);
    float platformHeight = groundPlatform.getTiles().front().getGlobalBounds().top;
    Player player(0, platformHeight - player.getGlobalBounds().height);

    // Clock for deltaTime
    sf::Clock clock;

    // Create TileManager for the level editor
    TileManager tileManager;

    // Set initial mode to Play Mode
    GameMode currentMode = GameMode::Play;

    // Help text to show controls
    sf::Text helpText = createHelpText(font, currentMode);

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            // Mode switching logic
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::E) {
                    currentMode = GameMode::Edit;  // Switch to editor mode
                    helpText = createHelpText(font, currentMode);  // Update help text
                } else if (event.key.code == sf::Keyboard::P) {
                    currentMode = GameMode::Play;  // Switch to play mode
                    helpText = createHelpText(font, currentMode);  // Update help text
                }
            }

            // Debug mode toggle (for grid lines, etc.)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                tileManager.toggleDebugMode();
            }
        }

        window.clear();

        // Handle the two game modes
        if (currentMode == GameMode::Play) {
            // **Play Mode Logic**
            float deltaTime = clock.restart().asSeconds();

            // Update player movement and logic
            player.update(deltaTime, { groundPlatform }, windowSize.x, windowSize.y);

            // Draw the ground platform
            groundPlatform.draw(window);

            // Draw the player
            player.draw(window);

            // Draw the orb counter
            window.draw(orbCounterText);
        } 
        else if (currentMode == GameMode::Edit) {
            // **Editor Mode Logic**
            clock.restart();  // Reset clock to avoid sudden jumps in play mode

            // Handle tile placement/input in the level editor
            tileManager.handleInput(window);

            // Draw the tiles in the editor
            tileManager.draw(window);

            // Add save/load functionality (optional)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                tileManager.saveLevel("levels/level1.txt");  // Save current layout
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
                tileManager.loadLevel("levels/level1.txt");  // Load saved layout
            }
        }

        // Draw help text (controls)
        window.draw(helpText);

        window.display();
    }

    return 0;
}

