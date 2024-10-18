#include "../include/Enemy.hpp"
#include "../include/Platform.hpp"
#include <iostream>

Enemy::Enemy(float startX, float startY)
    : x(startX), 
      y(startY), 
      yVelocity(0.0f), 
      gravity(2000.0f), 
      terminalVelocity(1000.0f), 
      speedX(200.0f),  
      orbCount(0), 
      walkingTexture(),  
      idleTexture(),     
      sprite(),          
      currentFrame(),    
      currentFrameIndex(0), 
      animationTimer(0.0f), 
      frameDuration(0.1f), 
      isIdle(true), 
      currentState(EnemyState::PATROLLING), 
      previousState(EnemyState::PATROLLING), 
      patrolStartX(startX - 200.0f),  
      patrolEndX(startX + 200.0f),    
      aggroRange(300.0f)
{
    if (!idleTexture.loadFromFile("assets/characters/enemies/wrathborn.png")) {
        std::cerr << "Error loading idle texture file" << std::endl;
    }
    if (!walkingTexture.loadFromFile("assets/characters/enemies/wrathborn_sprite_sheet.png")) {
        std::cerr << "Error loading walking texture file" << std::endl;
    }
    
    // Set initial sprite and frame
    sprite.setTexture(walkingTexture);
    currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentFrame);

    sprite.setPosition(x, y);
    sprite.setScale(2.0f, 2.0f);  
}

void Enemy::update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight, float scaleFactor) {
    animationTimer += deltaTime;

    if (animationTimer >= frameDuration) {
        currentFrameIndex = (currentFrameIndex + 1) % totalFrames;
        currentFrame.left = currentFrameIndex * frameWidth;
        sprite.setTextureRect(currentFrame);
        animationTimer = 0.0f;
    }

    switch (currentState) {
        case EnemyState::IDLE:
            if (previousState != currentState) {
                sprite.setTexture(idleTexture); 
                resetAnimation();
            }
            break;
        case EnemyState::PATROLLING:
            updatePatrolling(deltaTime);
            if (previousState != currentState) {
                sprite.setTexture(walkingTexture);  
                resetAnimation();
            }
            break;
        case EnemyState::AGGRO:
            break;
        case EnemyState::ATTACKING:
            break;
    }

    previousState = currentState;

    sprite.setPosition(x, y);
    move(deltaTime, platforms, windowWidth, windowHeight, scaleFactor);
}

void Enemy::updatePatrolling(float deltaTime) {
    if (x <= patrolStartX) {
        x = patrolStartX;
        speedX = std::abs(speedX);  
    } else if (x >= patrolEndX) {
        x = patrolEndX;
        speedX = -std::abs(speedX);  
    }
    x += speedX * deltaTime;
}

void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::FloatRect Enemy::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Enemy::move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight, float scaleFactor) {
    sprite.setPosition(x, y);
    boundDetection(windowWidth, windowHeight);
}

void Enemy::boundDetection(int windowWidth, int windowHeight) {
    float enemyWidth = sprite.getGlobalBounds().width;
    float enemyHeight = sprite.getGlobalBounds().height;

    if (x < 0) {
        x = 0;
        speedX = std::abs(speedX);  
    }
    if (x + enemyWidth > windowWidth) {
        x = windowWidth - enemyWidth;
    }

    if (y < 0) {
        y = 0;
        yVelocity = 0;
    }
    if (y + enemyHeight > windowHeight) {
        y = windowHeight - enemyHeight;
        yVelocity = 0;
    }

    sprite.setPosition(x, y);
    if (x + sprite.getGlobalBounds().width > windowWidth) {
        x = windowWidth - sprite.getGlobalBounds().width;
        speedX = -std::abs(speedX);  
    }
}

void Enemy::setState(EnemyState newState) {
    if (newState != currentState) {  
        currentState = newState;
        resetAnimation();  
    }
}

Enemy::EnemyState Enemy::getState() const {
    return currentState;
}

void Enemy::resetAnimation() {
    currentFrameIndex = 0;
    animationTimer = 0.0f;
    currentFrame.left = 0;
    sprite.setTextureRect(currentFrame);
}

