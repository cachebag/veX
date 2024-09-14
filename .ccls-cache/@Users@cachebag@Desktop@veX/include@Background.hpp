#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <SFML/Graphics.hpp>

class Background {
public:
    Background(const std::string& backgroundFilePath, const std::string& middlegroundFilePath, const sf::Vector2u& windowSize);
    void render(sf::RenderWindow& window, const sf::Vector2u& windowSize);

private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Texture middlegroundTexture;
    sf::Sprite middlegroundSprite;
};

#endif // BACKGROUND_HPP
