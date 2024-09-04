// Platform.hpp
#pragma once
#include <SFML/Graphics.hpp>

class Platform {
public:
    Platform(float x, float y, float width, float height, sf::Color color = sf::Color::Green);

    sf::FloatRect getBounds() const;
    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape shape;
};

