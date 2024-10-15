#include "../include/Enemy.hpp"
#include "../include/Platform.hpp"
#include <iostream>

Enemy::Enemy(float startX, float startY)
    : x(startX), 
      y(startY), 
      yVelocity(0.0f), 
      gravity(2000.0f), 
      terminalVelocity(1000.0f), 
      speedX(200.0f),  // Adjust speed for smooth patrolling
      orbCount(0), 
      walkingTexture(),  // Default initialization for sf::Texture
      idleTexture(),     // Default initialization for sf::Texture
      sprite(),          // Default initialization for sf::Sprite
      currentFrame(),    // Default initialization for sf::IntRect
      currentFrameIndex(0), 
      animationTimer(0.0f), 
      frameDuration(0.1f), 
      isIdle(true), 
      currentState(EnemyState::PATROLLING), 
      patrolStartX(startX - 200.0f),  // Define patrol range relative to start position
      patrolEndX(startX + 200.0f),    // Patrol 400 units in total
      aggroRange(300.0f)
{
    if (!idleTexture.loadFromFile("assets/characters/enemies/wrathborn.png")) {
        std::cerr << "Error loading idle texture file" << std::endl;
    }
    if (!walkingTexture.loadFromFile("assets/characters/enemies/wrathborn_sprite_sheet.png")) {
        std::cerr << "Error loading walking texture file" << std::endl;
    }
    sprite.setTexture(idleTexture);

    currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentFrame);

    sprite.setPosition(x, y);
    sprite.setScale(2.0f, 2.0f);  // Adjust scale
}

void Enemy::update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    animationTimer += deltaTime;

    if (animationTimer >= frameDuration) {
        currentFrameIndex = (currentFrameIndex + 1) % totalFrames;
        currentFrame.left = currentFrameIndex * frameWidth;
        sprite.setTextureRect(currentFrame);
        animationTimer = 0.0f;
    }

    switch (currentState) {
        case EnemyState::IDLE:
            break;
        case EnemyState::PATROLLING:
            updatePatrolling(deltaTime);
            break;
        case EnemyState::AGGRO:
            break;
        case EnemyState::ATTACKING:
            break;
    }

    sprite.setPosition(x, y);
    move(deltaTime, platforms, windowWidth, windowHeight);
}

void Enemy::updatePatrolling(float deltaTime) {
    // Update patrol movement between patrolStartX and patrolEndX
    if (x <= patrolStartX) {
        x = patrolStartX;
        speedX = std::abs(speedX);  // Move right
        sprite.setTexture(walkingTexture);
    } else if (x >= patrolEndX) {
        x = patrolEndX;
        speedX = -std::abs(speedX);  // Move left
        sprite.setTexture(walkingTexture);
    }
    x += speedX * deltaTime;
}

void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::FloatRect Enemy::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Enemy::move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    sprite.setPosition(x, y);
    boundDetection(windowWidth, windowHeight);
}

void Enemy::boundDetection(int windowWidth, int windowHeight) {
    if (x < 0) {
        x = 0;
        speedX = std::abs(speedX);  // Reverse direction when reaching window boundary
    }
    if (x + sprite.getGlobalBounds().width > windowWidth) {
        x = windowWidth - sprite.getGlobalBounds().width;
        speedX = -std::abs(speedX);  // Reverse direction when reaching window boundary
    }
}

