#include "../include/SidebarManager.hpp"
#include <iostream>

SidebarManager::SidebarManager(TileManager& tileManager)
    : tileManager(tileManager) {
    loadFont();
}

void SidebarManager::loadFont() {
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Error loading font for sidebar buttons." << std::endl;
    }
}

void SidebarManager::draw(sf::RenderWindow& sidebarWindow) {
    sidebarWindow.clear(sf::Color(50, 50, 50));

    drawTileButtons(sidebarWindow);
    drawSaveLoadButtons(sidebarWindow);

    sidebarWindow.display();
}

void SidebarManager::drawTileButtons(sf::RenderWindow& sidebarWindow) {
    float x = 20.0f;
    float y = 20.0f;

    for (const auto& [tileType, texture] : tileManager.getTileTextures()) {
        sf::RectangleShape tileButton(sf::Vector2f(static_cast<float>(tileButtonSize), static_cast<float>(tileButtonSize)));
        tileButton.setPosition(x, y);
        tileButton.setTexture(&texture);

        if (!tileManager.getPlacedTiles().empty() && tileType == tileManager.getPlacedTiles().front().type) {
            tileButton.setOutlineColor(sf::Color::Red);
        } else {
            tileButton.setOutlineColor(sf::Color::Transparent);
        }

        tileButton.setOutlineThickness(2);
        sidebarWindow.draw(tileButton);

        y += tileButtonSize + 10.0f;
    }
}

void SidebarManager::drawSaveLoadButtons(sf::RenderWindow& sidebarWindow) {
    sf::RectangleShape saveButton(sf::Vector2f(150, 50));
    saveButton.setPosition(20, static_cast<float>(sidebarWindow.getSize().y) - 150);
    saveButton.setFillColor(sf::Color::Blue);
    saveButton.setOutlineThickness(2);
    saveButton.setOutlineColor(sf::Color::White);
    sidebarWindow.draw(saveButton);

    sf::Text saveText("Save", font, 20);
    saveText.setPosition(60, static_cast<float>(sidebarWindow.getSize().y) - 140);
    sidebarWindow.draw(saveText);

    sf::RectangleShape loadButton(sf::Vector2f(150, 50));
    loadButton.setPosition(20, static_cast<float>(sidebarWindow.getSize().y) - 80);
    loadButton.setFillColor(sf::Color::Green);
    loadButton.setOutlineThickness(2);
    loadButton.setOutlineColor(sf::Color::White);
    sidebarWindow.draw(loadButton);

    sf::Text loadText("Load", font, 20);
    loadText.setPosition(60, static_cast<float>(sidebarWindow.getSize().y) - 70);
    sidebarWindow.draw(loadText);
}

void SidebarManager::handleInput(sf::RenderWindow& sidebarWindow) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(sidebarWindow);

        float x = 20.0f;
        float y = 20.0f;

        for (const auto& [tileType, texture] : tileManager.getTileTextures()) {
            sf::RectangleShape tileButton(sf::Vector2f(static_cast<float>(tileButtonSize), static_cast<float>(tileButtonSize)));
            tileButton.setPosition(x, y);

            if (tileButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                tileManager.setSelectedTile(tileType);
            }

            y += tileButtonSize + 10.0f;
        }

        sf::RectangleShape saveButton(sf::Vector2f(150, 50));
        saveButton.setPosition(20, static_cast<float>(sidebarWindow.getSize().y) - 150);

        if (saveButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            tileManager.saveLevel();
        }

        sf::RectangleShape loadButton(sf::Vector2f(150, 50));
        loadButton.setPosition(20, static_cast<float>(sidebarWindow.getSize().y) - 80);

        if (loadButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            tileManager.loadLevel();
        }
    }
}

