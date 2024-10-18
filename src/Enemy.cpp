#include "../include/Enemy.hpp"
#include "../include/Platform.hpp"
#include <iostream>

Enemy::Enemy(float startX, float startY)
    : x(startX), 
      y(startY), 
      yVelocity(0.0f), 
      gravity(2000.0f), 
      terminalVelocity(1000.0f), 
      speedX(0.0f),  // Enemy doesn't move
      orbCount(0), 
      walkingTexture(),  
      idleTexture(),     
      sprite(),          
      currentFrame(),    
      currentFrameIndex(0), 
      animationTimer(0.0f), 
      frameDuration(0.1f), 
      isIdle(true), 
      currentState(EnemyState::IDLE), 
      previousState(EnemyState::IDLE)
{
    // Load the walking texture
    if (!walkingTexture.loadFromFile("assets/characters/enemies/wrathborn_sprite_sheet.png")) {
        std::cerr << "Error loading walking texture file" << std::endl;
    }

    // Set the walking texture as the active texture even in idle state
    sprite.setTexture(walkingTexture);
    currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentFrame);
    
    sprite.setPosition(startX, startY);  // Enemy's position will be based on main.cpp
    sprite.setScale(2.0f, 2.0f);  
}

void Enemy::update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    animationTimer += deltaTime;

    // Update animation frames to play the walking animation even when idle
    if (animationTimer >= frameDuration) {
        currentFrameIndex = (currentFrameIndex + 1) % totalFrames;
        currentFrame.left = currentFrameIndex * frameWidth;
        sprite.setTextureRect(currentFrame);
        animationTimer = 0.0f;
    }

    // Ensure the enemy stays idle at the current position
    sprite.setPosition(x, y);
}

void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(sprite);  // Draw the sprite
}

sf::FloatRect Enemy::getGlobalBounds() const {
    return sprite.getGlobalBounds();  // Get the bounding box for collision
}

void Enemy::move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    // No movement for the idle state
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

