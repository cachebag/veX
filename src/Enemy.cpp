#include "../include/Enemy.hpp"
#include "../include/Platform.hpp"
#include <iostream>

Enemy::Enemy(float startX, float startY)
    : x(startX), y(startY), 
      yVelocity(0.0f), gravity(0.0f), terminalVelocity(1000.0f),
      speedX(100.0f),
      orbCount(0),
      currentState(EnemyState::PATROLLING),
      patrolStartX(startX - 100.0f),
      patrolEndX(startX + 100.0f),
      aggroRange(300.0f),
      currentFrameIndex(0),
      animationTimer(0.0f),
      frameDuration(0.1f),
      isIdle(true)
{
    if (!idleTexture.loadFromFile("assets/characters/enemies/wrathborn.png")) {
        std::cerr << "Error loading idle texture file" << std::endl;
    }
    if (!walkingTexture.loadFromFile("assets/characters/enemies/wrathborn_sprite_sheet.png")) {
	    std::cerr << "Error loading walkikng texture file" << std::endl;
    }
    sprite.setTexture(idleTexture);

    currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentFrame);

    sprite.setPosition(x, y);
    sprite.setScale(2.0f, 2.0f);
}

void Enemy::update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    (void)platforms;
    
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

void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::FloatRect Enemy::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Enemy::collectOrb() {
    orbCount++;
}

int Enemy::getOrbCount() const {
    return orbCount;
}

void Enemy::changeState(EnemyState newState) {
    currentState = newState;
}

void Enemy::updatePatrolling(float deltaTime) {
    if (x <= patrolStartX) {
        x = patrolStartX;
        speedX = std::abs(speedX);
	    sprite.setTexture(walkingTexture);
    } else if (x >= patrolEndX) {
        x = patrolEndX;
        speedX = -std::abs(speedX);
	    sprite.setTexture(walkingTexture);
    }
    x += speedX * deltaTime;
}

void Enemy::move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    (void)deltaTime;
    (void)platforms;
    (void)windowHeight;

    sprite.setPosition(x, y);
    boundDetection(windowWidth, windowHeight);
}

void Enemy::boundDetection(int windowWidth, int windowHeight) {
    (void)windowHeight;

    if (x < 0) {
        x = 0;
        speedX = std::abs(speedX);
    }
    if (x + sprite.getGlobalBounds().width > windowWidth) {
        x = windowWidth - sprite.getGlobalBounds().width;
        speedX = -std::abs(speedX);
    }
}

