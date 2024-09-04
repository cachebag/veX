#include <iostream>
#include <SFML/Graphics.hpp>
#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include <vector>

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

    // Common game setup (works for both fullscreen and windowed modes)
    window.setFramerateLimit(120);

    Player player(0, 1000);

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

        // Clear the window and draw everything
        window.clear();
        for (const auto& platform : platforms) {
            platform.draw(window);
        }
        player.draw(window);
        window.display();
    }

    return 0;
}

