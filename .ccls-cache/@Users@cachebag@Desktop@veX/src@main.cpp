// main.cpp
#include <SFML/Graphics.hpp>
#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include <vector>

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "veX");
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
        float deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        player.update(deltaTime, platforms);

        window.clear();
        for (const auto& platform : platforms) {
            platform.draw(window);
        }

        player.draw(window);
        window.display();
    }

    return 0;
}

