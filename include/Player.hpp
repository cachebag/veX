#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp"
#include "Enemy.hpp"  

class Player {
public:
    Player(float startX = 0.0f, float startY = 500.0f);

    void update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight, Enemy& enemy);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getGlobalBounds() const;

    void collectOrb();
    int getOrbCount() const;

    // New methods to get and set position
    sf::Vector2f getPosition() const;
    void setPosition(float newX, float newY);
    void resetState();

private:
    float x, y;               
    float prevX, prevY;        
    float yVelocity;
    const float gravity;
    const float terminalVelocity;
    const float speedX;
    const float jumpVelocity;

    int jumpCount;
    const int maxJumps;
    const float fallMultiplier;
    const float lowJumpMultiplier;

    int orbCount;

    sf::Texture walkingTexture;
    sf::Texture idleTexture;
    sf::Texture jumpTexture;
    sf::Sprite sprite;
    sf::IntRect currentFrame;

    int currentFrameIndex;
    float animationTimer;
    float frameDuration;
    bool isIdle;
    bool canJump;
    bool isJumping;

    const int frameWidth = 32;
    const int frameHeight = 32;
    int totalFrames = 3;
    const int idleTotalFrames = 3;

    void handleInput(float deltaTime);
    void applyGravity(float deltaTime);
    void move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight, Enemy& enemy);
    void boundDetection(int windowWidth, int windowHeight);
    void enemyDetection(Enemy& enemy);  
    void resetAnimation();
};

