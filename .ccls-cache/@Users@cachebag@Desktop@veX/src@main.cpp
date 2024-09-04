#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include <vector>
#include "../include/Orb.hpp" 

int main() {
    // Get a list of all valid fullscreen modes
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();

    sf::RenderWindow window;
    
    if (modes.empty()) {
        std::cerr << "Error: No valid fullscreen modes available, running in windowed mode (1920x1080)." << std::endl;
        
        // Fallback to windowed mode with 1920x1080 resolution
        sf::VideoMode windowedMode(1920, 1080);
        window.create(windowedMode, "veX", sf::Style::Default);
    } else {
        // Use the first (most preferred) fullscreen mode
        sf::VideoMode fullScreenMode = modes[0];
        window.create(fullScreenMode, "veX", sf::Style::Fullscreen);
    }

    // Load font for displaying the orb counter
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    // Create the orb counter text object
    sf::Text orbCounterText;
    orbCounterText.setFont(font);  // Set the loaded font
    orbCounterText.setCharacterSize(24);  // Character size in pixels
    orbCounterText.setFillColor(sf::Color::White);  // Text color
    orbCounterText.setPosition(1700.0f, 20.0f);  // Position in the top-right corner

    // Player and orb initialization
    Player player(0, 1000);
    Orb orb(100.0f, 100.0f, 10.0f);

    // Level design: platforms
    std::vector<Platform> platforms = {
        Platform(0.0f, 1030.0f, 1920.0f, 50.0f),
        Platform(900.0f, 800.0f, 200.0f, 20.0f),
        Platform(600.0f, 600.0f, 200.0f, 20.0f),
        Platform(900.0f, 400.0f, 200.0f, 20.0f)
    };

    sf::Clock clock;

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        // Get the elapsed time for this frame (delta time)
        float deltaTime = clock.restart().asSeconds();

        // Update player
        player.update(deltaTime, platforms);

        // Check if orb is collected
        if (orb.isCollected(player.getGlobalBounds())) {
            player.collectOrb();
        }

        // Update the orb counter text
        orbCounterText.setString("Orbs: " + std::to_string(player.getOrbCount()));

        // Clear the window and draw everything
        window.clear();

        for (const auto& platform : platforms) {
            platform.draw(window);
        }

        player.draw(window);
        orb.draw(window);  // Draw the orb
        window.draw(orbCounterText);  // Draw the orb counter

        window.display();
    }

    return 0;
}

