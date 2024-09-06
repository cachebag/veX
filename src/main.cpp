#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include <vector>
#include "../include/Orb.hpp"
#include <cstdlib>
#include <ctime>

const int NUM_ORBS = 5;
const float ORB_RADIUS = 10.0f;

int main() {
    // Get a list of all valid fullscreen modes
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();

    sf::RenderWindow window;
    
    if (modes.empty()) {
        std::cerr << "Error: No valid fullscreen modes available, running in windowed mode (1920x1080)." << std::endl;
        
        // Fallback to windowed mode with 1920x1080 resolution
        sf::VideoMode windowedMode(2560, 1440);
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

    // initialize a random seed
    srand(static_cast<unsigned>(time(0)));
   
    std::vector<Orb> orbs;

    // 5 random orbs
    for (int i = 0; i < NUM_ORBS; ++i) {
        float x = static_cast<float>(rand() % 800);
        float y = static_cast<float>(rand() % 800);
        orbs.push_back(Orb(x, y, ORB_RADIUS));
    }

    // Player initialization
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
        
        std::vector<Orb> orbs;

        // 5 random orbs

        // Check if any orb is collected
        for (auto it = orbs.begin(); it != orbs.end(); ) {
            if (it->isCollected(player.getGlobalBounds())) {
                float x = static_cast<float>(rand() % 800);
                float y = static_cast<float>(rand() % 800);
                player.collectOrb();
                it = orbs.erase(it);  // Remove orb from the vector if collected
            } else {
                ++it;
            }
        }

        // Update the orb counter text
        orbCounterText.setString("Orbs: " + std::to_string(player.getOrbCount()));

        // Clear the window and draw everything
        window.clear();

        for (const auto& platform : platforms) {
            platform.draw(window);
        }

        player.draw(window);

        // Draw each remaining orb
        for (const auto& orb : orbs) {
            orb.draw(window);
        }

        // Draw the orb counter
        window.draw(orbCounterText);

        window.display();
    }

    return 0;
}

