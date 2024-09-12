#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <SFML/Graphics.hpp>

class Background {
public:
    Background(const std::string& filePath);
    void render(sf::RenderWindow& window, const sf::Vector2u& windowSize);

private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
};

#endif // BACKGROUND_HPP
