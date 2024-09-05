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

    void boundDetection(int windowWidth, int windowHeight);

    void collectOrb();
    int getOrbCount() const;

private:
    float x, y;
    float yVelocity;
    const float gravity;
    const float terminalVelocity;
    const float speedX;
    const float jumpVelocity;

    // New variables for jump and gravity mechanics
    int jumpCount;            // Track the number of jumps (for double jump)
    const int maxJumps;       // Maximum allowed jumps (2 for double jump)
    const float fallMultiplier;      // Multiplier to make falling faster
    const float lowJumpMultiplier;   // Multiplier to control jump height

    int orbCount;
    sf::RectangleShape shape;

    void handleInput(float deltaTime);
    void applyGravity(float deltaTime);
    void move(float deltaTime, const std::vector<Platform>& platforms);
};

