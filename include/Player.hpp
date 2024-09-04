// Player.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp" // Include Platform class for collision detection

class Player {
public:
    Player(float startX = 0.0f, float startY = 500.0f);
    
    void update(float deltaTime, const std::vector<Platform>& platforms);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getGlobalBounds() const;

private:
    float x, y;
    float yVelocity;
    const float gravity;
    const float terminalVelocity;
    const float speedX;
    const float jumpVelocity;
    sf::RectangleShape shape;

    void handleInput(float deltaTime);
    void applyGravity(float deltaTime);
    void move(float deltaTime, const std::vector<Platform>& platforms);
};

