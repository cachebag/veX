#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Platform {
public:
    Platform(float x, float y, float width, float height, const sf::Texture& texture, bool isGrassy = false);

    void draw(sf::RenderWindow& window);  
    sf::FloatRect getBounds() const;      

    const std::vector<sf::RectangleShape>& getTiles() const;  
    void rescale(float scaleX, float scaleY);  

    bool checkCollision(const sf::FloatRect& playerBounds) const; 

private:
    std::vector<sf::RectangleShape> tiles;  
    sf::FloatRect collisionBounds;  
};

