#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <SFML/Graphics.hpp>

class Background {
public:
    Background(const std::string& backgroundFilePath, const std::string& middlegroundFilePath, const std::string& mountaintsFilePath, const sf::Vector2u& windowSize);
    void render(sf::RenderWindow& window, const sf::Vector2u& windowSize, float playerX, float deltaTime);

private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Texture middlegroundTexture;
    sf::Sprite middlegroundSprite;

    sf::Texture mountainsTexture;
    sf::Sprite mountainsSprite;
};

#endif // BACKGROUND_HPP
