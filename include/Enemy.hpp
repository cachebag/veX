#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp"

class Enemy {
public:
    enum class EnemyState {
        IDLE,
        PATROLLING,
        AGGRO,
        ATTACKING,
        BOSS_FIGHT  // New state for boss fight mode
    };

    Enemy(float startX = 500.0f, float startY = 500.0f);
    void update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getGlobalBounds() const;
    EnemyState getState() const;
    void setState(EnemyState newState);
    bool checkCollision(const sf::FloatRect& otherBounds) const;
    void move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    sf::Vector2f getPosition() const;
    void setPosition(float newX, float newY);
    void flipSprite();

    // New methods for boss fight
    void updateBossFight(float deltaTime, float minX, float maxX);
    void shootOrb();
    void takeDamage(float amount);
    float getHealth() const { return health; }
    void resetHealth() { health = maxHealth; }
    void setMovementBounds(float minX, float maxX) { 
        movementBoundLeft = minX; 
        movementBoundRight = maxX; 
    }
    bool isBossFight() const { return currentState == EnemyState::BOSS_FIGHT; }
    void startBossFight();

private:
    // Existing members
    float x, y;
    float yVelocity;
    const float gravity;
    const float terminalVelocity;
    float speedX;
    bool isFacingRight;
    int orbCount;
    sf::Texture walkingTexture;
    sf::Texture idleTexture;
    sf::Sprite sprite;
    sf::IntRect currentFrame;
    int currentFrameIndex;
    float animationTimer;
    float frameDuration;
    bool isIdle;
    const int frameWidth = 64;
    const int frameHeight = 64;
    int totalFrames = 6;
    const int idleTotalFrames = 3;
    EnemyState currentState;
    EnemyState previousState;

    // New members for boss fight
    float health;
    const float maxHealth = 100.0f;
    float movementBoundLeft;
    float movementBoundRight;
    float bossMovementSpeed = 200.0f;
    float direction = 1.0f;  // 1 for right, -1 for left
    
    // Helper methods
    void resetAnimation();
    void updateBossMovement(float deltaTime);
    void updateAnimation(float deltaTime);
    void handleStateTransition();
};
