#ifndef TITLESCREEN_HPP
#define TITLESCREEN_HPP

#include <SFML/Graphics.hpp>
#include "Background.hpp"  

class TitleScreen {
public:
    TitleScreen(sf::RenderWindow& window);
    void loadAssets();
    void handleInput();
    void update(float deltaTime);  
    void render();

    int currentSelection = 0;

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text gameTitle;
    sf::Text menuOptions[3];
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    Background background;  
};

#endif

