#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp" 

class Enemy {
public:
    Enemy(float startX = 500.0f, float startY = 500.0f);

    void update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getGlobalBounds() const;

    void collectOrb();
    int getOrbCount() const;

private:
    float x, y; 
    float yVelocity; 
    const float gravity; 
    const float terminalVelocity; 
    float speedX; 

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
    int totalFrames = 3;
    const int idleTotalFrames = 3;

    enum class EnemyState {
        IDLE,
        PATROLLING,
        AGGRO,
        ATTACKING
    };

    EnemyState currentState; 
    float patrolStartX, patrolEndX; 
    float aggroRange; 

    void resetAnimation();
    void changeState(EnemyState newState);
    void updatePatrolling(float deltaTime);
    void move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void boundDetection(int windowWidth, int windowHeight);
};

