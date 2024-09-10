#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include <iostream>

// Constructor
Player::Player(float startX, float startY) 
    : x(startX), y(startY), 
      yVelocity(0.0f), gravity(2000.0f), terminalVelocity(1000.0f), 
      speedX(600.0f), jumpVelocity(-1100.0f), 
      jumpCount(0), maxJumps(2), 
      fallMultiplier(2.5f), lowJumpMultiplier(1.5f), 
      orbCount(0),
      currentFrameIndex(0),
      animationTimer(0.0f),
      frameDuration(0.1f),
      isIdle(true),
      canJump(true),
      isJumping(false)
{
    // Load the walking sprite sheet
    if (!walkingTexture.loadFromFile("assets/characters/player/veX_sprite_sheet.png")) {
        std::cerr << "Error loading walking texture file" << std::endl;
    }

    // Load the idle sprite sheet
    if (!idleTexture.loadFromFile("assets/characters/player/veX_breathe_sheet.png")) {
        std::cerr << "Error loading idle texture file" << std::endl;
    }

    // Load jumping frame
    if (!jumpTexture.loadFromFile("assets/characters/player/veX_jump.png")) {
        std::cerr << "Error loading jump texture file" << std::endl;
    }
    
    // Set the initial texture to idle
    sprite.setTexture(idleTexture);

    // Set the initial frame for the sprite (32x32 size for each frame)
    currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);  
    sprite.setTextureRect(currentFrame);

    // Set initial sprite position and scale (if needed)
    sprite.setPosition(x, y);
    sprite.setScale(2.0f, 2.0f);  // Scale to make the character larger if needed
}

void Player::update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    handleInput(deltaTime);
    applyGravity(deltaTime);
    move(deltaTime, platforms, windowWidth, windowHeight);

    // Update animation frame based on time
    animationTimer += deltaTime;

    if (animationTimer >= frameDuration) {
        currentFrameIndex = (currentFrameIndex + 1) % totalFrames;
        currentFrame.left = currentFrameIndex * frameWidth;  // Move horizontally through the sprite sheet
        sprite.setTextureRect(currentFrame);  // Apply the new frame to the sprite
        animationTimer = 0.0f;
    }

    // Update sprite position to match player movement
    sprite.setPosition(x, y);
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(sprite);  // Draw the player sprite
}

sf::FloatRect Player::getGlobalBounds() const {
    return sprite.getGlobalBounds();  // Return the global bounds of the sprite
}

void Player::collectOrb() {
    orbCount++;
}

int Player::getOrbCount() const {
    return orbCount;
}

void Player::handleInput(float deltaTime) {
    float velocityX = 0.0f;  // Horizontal velocity starts at 0
    bool isMoving = false;   // Track if the player is moving

    // Jump logic
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        if (canJump && jumpCount < maxJumps) {
            yVelocity = jumpVelocity;  // Apply jump velocity
            jumpCount++;  // Increment jump count
            canJump = false;
            isJumping = true;
            sprite.setTexture(jumpTexture);
            resetAnimation();
        }
    } else {
        canJump = true;  // Allow jumping when space is released
    }

    // Move left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocityX = -speedX;
        isMoving = true;  // Player is moving

        if (sprite.getScale().x > 0) {
            sprite.setScale(-2.0f, 2.0f);  // Flip horizontally
            sprite.setOrigin(frameWidth, 0);  // Adjust origin to keep position consistent
        }

        // Only switch to walking texture if transitioning from idle
        if (isIdle) {
            sprite.setTexture(walkingTexture);
            resetAnimation();  // Reset animation when switching to walking
            isIdle = false;    // Mark player as moving
        }
    }
    // Move right
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocityX = speedX;
        isMoving = true;  // Player is moving

        // Flip sprite to face right
        if (sprite.getScale().x < 0) {
            sprite.setScale(2.0f, 2.0f);  // Flip horizontally back to normal
            sprite.setOrigin(0, 0);  // Reset origin to default
        }

        // Only switch to walking texture if transitioning from idle
        if (isIdle) {
            sprite.setTexture(walkingTexture);
            resetAnimation();  // Reset animation when switching to walking
            isIdle = false;    // Mark player as moving
        }
    }

    // Apply horizontal velocity if any
    x += velocityX * deltaTime;

    if (yVelocity >= 0) {
        isJumping = false;
    }

    // Set idle texture when not moving and not jumping
    if (!isMoving && !isJumping) {
        if (!isIdle) {
            sprite.setTexture(idleTexture);     // Switch to idle texture
            totalFrames = idleTotalFrames;      // Idle animation
            resetAnimation();                   // Reset animation when switching to idle
            isIdle = true;                      // Mark player as idle
        }
    }

    if (isJumping) {
        sprite.setTexture(jumpTexture);
        resetAnimation();
    } else if (isMoving) {
        sprite.setTexture(walkingTexture);
    }
}

void Player::applyGravity(float deltaTime) {
    // Apply gravity with different effects based on rising or falling
    if (yVelocity > 0) {
        // Falling
        yVelocity += gravity * fallMultiplier * deltaTime;
    } else if (yVelocity < 0) {
        // Rising or jumping
        yVelocity += gravity * lowJumpMultiplier * deltaTime;
    } else {
        // Neutral velocity (e.g., standing or at peak jump)
        yVelocity += gravity * deltaTime;
    }

    // Cap velocity to terminal velocity
    if (yVelocity > terminalVelocity) {
        yVelocity += gravity * deltaTime;
    }
}

void Player::boundDetection(int windowWidth, int windowHeight) {
    float playerWidth = sprite.getGlobalBounds().width;
    float playerHeight = sprite.getGlobalBounds().height;

    // Horizontal bounds
    if (x < 0) {
        x = 0;
    }
    if (x + playerWidth > windowWidth) {
        x = windowWidth - playerWidth;
    }

    // Vertical bounds
    if (y < 0) {
        y = 0;
        yVelocity = 0;
    }
    if (y + playerHeight > windowHeight) {
        y = windowHeight - playerHeight;
        yVelocity = 0;
        jumpCount = 0;  // Reset jump count if hitting the bottom
    }

    sprite.setPosition(x, y);
}

void Player::move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight) {
    // Move the player vertically by applying yVelocity
    y += yVelocity * deltaTime;

    sf::FloatRect playerBounds = sprite.getGlobalBounds();
    playerBounds.top = y;

    bool onGround = false;

    // Check for collisions with platforms
    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform.getBounds();

        // Check for top collision (landing on top of the platform)
        if (playerBounds.intersects(platformBounds)) {
            if (yVelocity > 0.0f && (playerBounds.top + playerBounds.height) <= platformBounds.top + 5) {
                y = platformBounds.top - playerBounds.height;  // Position player on top of the platform
                yVelocity = 0.0f;  // Stop falling
                onGround = true;    // Player is on the ground
            }
            // Handle bottom collision (hitting the platform from below)
            else if (yVelocity < 0.0f && playerBounds.top >= platformBounds.top + platformBounds.height - 5) {
                y = platformBounds.top + platformBounds.height;  // Position player below the platform
                yVelocity = 0.0f;  // Stop upward motion
            }
        }
    }

    // If the player is on the ground, reset the jump count
    if (onGround) {
        jumpCount = 0;
        canJump = true;
    }

    // Update the sprite's position to match the player's new position
    sprite.setPosition(x, y);
    boundDetection(windowWidth, windowHeight);
}

// Helper to reset the animation
void Player::resetAnimation() {
    currentFrameIndex = 0;
    animationTimer = 0.0f;
    currentFrame.left = 0;  // Reset to the first frame
    sprite.setTextureRect(currentFrame);
}

