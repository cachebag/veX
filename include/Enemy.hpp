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
        ATTACKING
    };

    Enemy(float startX = 500.0f, float startY = 500.0f);  // Use startX and startY from main.cpp

    void update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getGlobalBounds() const;

    EnemyState getState() const;
    void setState(EnemyState newState);

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
    int totalFrames = 6;
    const int idleTotalFrames = 3;

    EnemyState currentState;  
    EnemyState previousState; 

    void move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void resetAnimation();
};

