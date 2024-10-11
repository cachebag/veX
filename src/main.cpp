#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>  // For std::unique_ptr
#include "../include/Player.hpp"
#include "../include/Enemy.hpp"
#include "../include/Background.hpp"

enum class GameMode { Play };

sf::Text createHelpText(const sf::Font& font) {
    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(24);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(20.0f, 20.0f);
    helpText.setString(
        "Play Mode\n"
        "WASD to move, Space to jump\n"
        "Press ESC to quit"
    );
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

    std::unique_ptr<Player> player = std::make_unique<Player>(0, 0);  // Initial position of player
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(2400, 1100); // Initial position of enemy

    GameMode currentMode = GameMode::Play;
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

        float playerX = player->getGlobalBounds().left;
        float deltaTime = clock.restart().asSeconds();

        background.render(window, window.getSize(), playerX, deltaTime);

        if (currentMode == GameMode::Play) {
            player->update(deltaTime, std::vector<Platform>(), windowSize.x, windowSize.y);
            enemy->update(deltaTime, std::vector<Platform>(), windowSize.x, windowSize.y);

            player->draw(window);
            enemy->draw(window);
        }

        window.draw(helpText);
        window.display();
    }

    return 0;
}

