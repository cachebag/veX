#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include <iostream>

// Constructor
Player::Player(float startX, float startY) 
    : x(startX), y(startY), 
      yVelocity(0.0f), gravity(2000.0f), terminalVelocity(1000.0f), 
      speedX(500.0f), jumpVelocity(-1500.0f), 
      jumpCount(0), maxJumps(2), 
      fallMultiplier(2.5f), lowJumpMultiplier(1.5f), 
      orbCount(0),
      currentFrameIndex(0),
      animationTimer(0.0f),
      frameDuration(0.1f)  // Set frame duration for animation
{
    // Load the sprite sheet and set the initial frame
    if (!texture.loadFromFile("assets/characters/player/veX_sprite_sheet.png")) {
        std::cerr << "Error loading texture file" << std::endl;
    }
    sprite.setTexture(texture);

    // Set the initial frame for the sprite (32x32 size for each frame)
    currentFrame = sf::IntRect(0, 0, 32, 32);  
    sprite.setTextureRect(currentFrame);

    // Set initial sprite position and scale (if needed)
    sprite.setPosition(x, y);
    sprite.setScale(5.0f, 5.0f);  // Scale to make the character larger if needed
}

void Player::update(float deltaTime, const std::vector<Platform>& platforms) {
    handleInput(deltaTime);
    applyGravity(deltaTime);
    move(deltaTime, platforms);

    // Update animation frame based on time
    animationTimer += deltaTime;
    if (animationTimer >= frameDuration) {
        // Cycle through 4 frames (0-3)
        currentFrameIndex = (currentFrameIndex + 1) % 4;
        currentFrame.left = currentFrameIndex * 32;  // Move horizontally through the sprite sheet (32x32 frames)
        sprite.setTextureRect(currentFrame);  // Apply the new frame to the sprite
        animationTimer = 0.0f;  // Reset the animation timer
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

bool canJump = true;

void Player::handleInput(float deltaTime) {
    // Stop horizontal movement when no keys are pressed
    float velocityX = 0.0f;

    // Move left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocityX = -speedX;
        sprite.setScale(5.0f, 5.0f);  // Flip the sprite when moving left
    }

    // Move right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocityX = speedX;
        sprite.setScale(5.0f, 5.0f);  // Default scale for moving right
    }

    // Update position
    x += velocityX * deltaTime;

    // Jump logic
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        if (canJump && jumpCount < maxJumps) {
            yVelocity = jumpVelocity;  // Apply jump velocity
            jumpCount++;  // Increment jump count
            canJump = false;
        }
    } else {
        canJump = true;
    }

    // Update sprite position
    sprite.setPosition(x, y);
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
        yVelocity = terminalVelocity;
    }
}

void Player::move(float deltaTime, const std::vector<Platform>& platforms) {
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

        // Horizontal collision detection
        if (playerBounds.intersects(platformBounds)) {
            // Collision on the left or right side
            if (playerBounds.left < platformBounds.left) {
                x = platformBounds.left - playerBounds.width;  // Left side collision
            } else if (playerBounds.left + playerBounds.width > platformBounds.left + platformBounds.width) {
                x = platformBounds.left + platformBounds.width;  // Right side collision
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
    boundDetection(1920, 1080);  // Ensure player stays within the window bounds
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

