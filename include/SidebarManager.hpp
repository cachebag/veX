#pragma once
#include <SFML/Graphics.hpp>
#include "TileManager.hpp"

class SidebarManager {
public:
    SidebarManager(TileManager& tileManager);

    void draw(sf::RenderWindow& sidebarWindow);
    void handleInput(sf::RenderWindow& sidebarWindow);

private:
    TileManager& tileManager;
    int sidebarWidth = 200;
    int tileButtonSize = 64;

    sf::Font font;
    void loadFont();
    void drawTileButtons(sf::RenderWindow& sidebarWindow);
    void drawSaveLoadButtons(sf::RenderWindow& sidebarWindow);
};

