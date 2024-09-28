#include "../include/Enemy.hpp"
#include "../include/Platform.hpp"
#include <iostream>

// Constructor
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
    // Load the idle texture
    if (!idleTexture.loadFromFile("assets/characters/enemies/wrathborn.png")) {
        std::cerr << "Error loading idle texture file" << std::endl;
    }
    if (!walkingTexture.loadFromFile("assets/characters/enemies/wrathborn_sprite_sheet.png")) {
	    std::cerr << "Error loading walkikng texture file" << std::endl;
    }
    // Set the initial texture to idle
    sprite.setTexture(idleTexture);

    currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentFrame);

    // Set initial sprite position and scale (if needed)
    sprite.setPosition(x, y);
    sprite.setScale(2.0f, 2.0f);  // Scale to make the character larger if needed
}

void Enemy::update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    (void)platforms; // Suppress unused parameter warning
    
    animationTimer += deltaTime;

    if (animationTimer >= frameDuration) {
        currentFrameIndex = (currentFrameIndex + 1) % totalFrames;
        currentFrame.left = currentFrameIndex * frameWidth;  // Move horizontally through the sprite sheet
        sprite.setTextureRect(currentFrame);  // Apply the new frame to the sprite
        animationTimer = 0.0f;

    }

    switch (currentState) {
        case EnemyState::IDLE:
            // For now, do nothing
            break;
        case EnemyState::PATROLLING:
            updatePatrolling(deltaTime);
            break;
        case EnemyState::AGGRO:
            // For now, do nothing
            break;
        case EnemyState::ATTACKING:
            // For now, do nothing
            break;
    }
    sprite.setPosition(x, y);
    move(deltaTime, platforms, windowWidth, windowHeight);
}

void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(sprite); // Draw the enemy sprite
}

sf::FloatRect Enemy::getGlobalBounds() const {
    return sprite.getGlobalBounds(); // Return the global bounds of the sprite
}

void Enemy::collectOrb() {
    orbCount++;
}

int Enemy::getOrbCount() const {
    return orbCount;
}

void Enemy::changeState(EnemyState newState) {
    currentState = newState;
    // Reset state-specific logic if needed (e.g., timers or animations)
}

void Enemy::updatePatrolling(float deltaTime) {
    // Move left and right within patrol boundaries
    if (x <= patrolStartX) {
        x = patrolStartX;
        speedX = std::abs(speedX); // Move right
	sprite.setTexture(walkingTexture);
    } else if (x >= patrolEndX) {
        x = patrolEndX;
        speedX = -std::abs(speedX); // Move left
	sprite.setTexture(walkingTexture);
    }
    x += speedX * deltaTime;
}

void Enemy::move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    (void)deltaTime;   // Suppress unused parameter warning
    (void)platforms;   // Suppress unused parameter warning
    (void)windowHeight; // Suppress unused parameter warning

    // Update sprite's position
    sprite.setPosition(x, y);
    boundDetection(windowWidth, windowHeight);
}

void Enemy::boundDetection(int windowWidth, int windowHeight) {
    (void)windowHeight; // Suppress unused parameter warning

    // Check bounds within the window
    if (x < 0) {
        x = 0;
        speedX = std::abs(speedX); // Move right
    }
    if (x + sprite.getGlobalBounds().width > windowWidth) {
        x = windowWidth - sprite.getGlobalBounds().width;
        speedX = -std::abs(speedX); // Move left
    }
    // Implement additional vertical bounds if necessary
}

