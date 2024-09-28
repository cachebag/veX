#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp" // Include Platform class for collision detection

class Enemy {
public:
    Enemy(float startX = 500.0f, float startY = 500.0f);

    void update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getGlobalBounds() const;

    void collectOrb();
    int getOrbCount() const;

private:
    float x, y; // Enemy position
    float yVelocity; // Vertical velocity (if needed)
    const float gravity; // Gravity (optional for some enemies)
    const float terminalVelocity; // Maximum falling speed (if applicable)
    float speedX; // Horizontal movement speed

    int orbCount; // Number of orbs collected (if applicable)

    sf::Texture idleTexture; // Texture for idle animation
    sf::Sprite sprite; // Sprite to represent the enemy

    enum class EnemyState {
        IDLE,
        PATROLLING,
        AGGRO,
        ATTACKING
    };

    EnemyState currentState; // Current state of the enemy
    float patrolStartX, patrolEndX; // Patrol boundaries
    float aggroRange; // Range to start chasing the player

    void changeState(EnemyState newState);
    void updatePatrolling(float deltaTime);
    void move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void boundDetection(int windowWidth, int windowHeight);
};

