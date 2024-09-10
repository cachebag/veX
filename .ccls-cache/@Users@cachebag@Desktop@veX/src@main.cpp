#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include <vector>

int main() {
    // Get desktop mode
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

    // Create the window
    sf::RenderWindow window(desktopMode, "veX", sf::Style::Default);
    sf::Vector2u windowSize = window.getSize();

    // Load font for orb counter
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

    // Load textures (non-const to allow modification)
    sf::Texture groundTexture1, groundTexture2, groundTexture3;
    if (!groundTexture1.loadFromFile("assets/tutorial_level/ground.png") ||
        !groundTexture2.loadFromFile("assets/tutorial_level/ground2.png") ||
        !groundTexture3.loadFromFile("assets/tutorial_level/ground3.png")) {
        std::cerr << "Error loading ground textures!" << std::endl;
        return -1;
    }

    // Player initialization
    Player player(0, 1000);

    // Create the new randomized platform with a hole in the middle
    Platform groundPlatform(0.0f, 0.9f, 1.0f, 0.1f, groundTexture1, groundTexture2, groundTexture3, windowSize);

    sf::Clock clock;

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        // Get deltaTime for the frame
        float deltaTime = clock.restart().asSeconds();

        // Update player
        player.update(deltaTime, {}, windowSize.x, windowSize.y);

        // Clear window and draw
        window.clear();
        groundPlatform.draw(window);  // Draw the randomized ground platform

        player.draw(window);

        window.draw(orbCounterText);

        window.display();
    }

    return 0;
}

