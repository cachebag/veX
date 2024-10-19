#include "../include/TitleScreen.hpp"
#include <stdexcept>

TitleScreen::TitleScreen(sf::RenderWindow& window)
    : window(window), background("assets/tutorial_level/background.png",
                                 "assets/tutorial_level/middleground.png",
                                 "assets/tutorial_level/mountains.png", sf::Vector2u(1920, 1080)) {
    loadAssets();
}

void TitleScreen::loadAssets() {
    if (!font.loadFromFile("assets/fonts/gothic.ttf")) {
        throw std::runtime_error("Could not load font: assets/fonts/gothic.ttf");
    }

    gameTitle.setFont(font);
    gameTitle.setString("veX");
    gameTitle.setCharacterSize(80);
    gameTitle.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = gameTitle.getLocalBounds();
    gameTitle.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    gameTitle.setPosition(window.getSize().x / 2, titleBounds.height / 2 + 50); 

    std::string options[] = { "Start Game", "Options", "Exit" };
    for (int i = 0; i < 3; ++i) {
        menuOptions[i].setFont(font);
        menuOptions[i].setString(options[i]);
        menuOptions[i].setCharacterSize(40);
        menuOptions[i].setFillColor(sf::Color::White);
        sf::FloatRect optionBounds = menuOptions[i].getLocalBounds();
        menuOptions[i].setOrigin(optionBounds.width / 2, optionBounds.height / 2);
        menuOptions[i].setPosition(window.getSize().x / 2, window.getSize().y / 2 + i * 60);
    }
}

void TitleScreen::handleInput() {
    static sf::Clock clock;
    if (clock.getElapsedTime().asSeconds() > 0.2f) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            currentSelection = (currentSelection + 1) % 3;
            clock.restart();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            currentSelection = (currentSelection - 1 + 3) % 3;
            clock.restart();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        if (currentSelection == 0) {
        } else if (currentSelection == 1) {
        } else if (currentSelection == 2) {
            window.close();
        }
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    for (int i = 0; i < 3; ++i) {
        if (menuOptions[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
            currentSelection = i;
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (currentSelection == 0) {
                    return;
                } else if (currentSelection == 1) {
                } else if (currentSelection == 2) {
                    window.close();
                }
            }
        }
    }
}

void TitleScreen::update(float deltaTime) {
    background.render(window, sf::Vector2u(1920, 1080), 0, deltaTime);  
    for (int i = 0; i < 3; ++i) {
        if (i == currentSelection) {
            menuOptions[i].setFillColor(sf::Color::Red);
        } else {
            menuOptions[i].setFillColor(sf::Color::White);
        }
    }
}

void TitleScreen::render() {
    window.clear();
    background.render(window, sf::Vector2u(1920, 1080), 0, 0);  
    window.draw(gameTitle);
    for (int i = 0; i < 3; ++i) {
        window.draw(menuOptions[i]);
    }
    window.display();
}

