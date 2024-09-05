#include "../include/Player.hpp"
#include "../include/Platform.hpp"

Player::Player(float startX, float startY) 
    : x(startX), y(startY), 
      yVelocity(0.0f), gravity(2000.0f), terminalVelocity(1000.0f), 
      speedX(500.0f), jumpVelocity(-1500.0f), 
      jumpCount(0), maxJumps(2), 
      fallMultiplier(2.5f), lowJumpMultiplier(1.5f), 
      orbCount(0) 
{
    shape.setSize(sf::Vector2f(50.0f, 50.0f));
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(x, y);
    boundDetection(1920, 1080);
}

void Player::update(float deltaTime, const std::vector<Platform>& platforms) {
    handleInput(deltaTime);
    applyGravity(deltaTime);
    move(deltaTime, platforms);
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

sf::FloatRect Player::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

void Player::collectOrb() {
    orbCount++;
}

int Player::getOrbCount() const {
    return orbCount;
}

bool canJump = true;

void Player::handleInput(float deltaTime) {
    // Move left and right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        x -= speedX * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        x += speedX * deltaTime;
    }

    shape.setPosition(x, y);

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

void Player::move(float deltaTime, const std::vector<Platform>& platforms) {
    // Move the player vertically by applying yVelocity
    y += yVelocity * deltaTime;

    sf::FloatRect playerBounds = shape.getGlobalBounds();
    playerBounds.top = y;

    bool onGround = false;

    // Check for collisions with platforms
    for (const auto& platform : platforms) {
        if (playerBounds.intersects(platform.getBounds())) {
            sf::FloatRect platformBounds = platform.getBounds();

            // Check if player is falling and collided with the top of a platform
            if (yVelocity > 0.0f && (playerBounds.top + playerBounds.height) <= platformBounds.top + 5) {
                y = platformBounds.top - playerBounds.height;  // Position player on top of the platform
                yVelocity = 0.0f;  // Stop falling
                onGround = true;    // Player is on the ground
            }
            else if (yVelocity < 0.0f && playerBounds.top >= platformBounds.top + platformBounds.height - 5) {
                y = platformBounds.top + platformBounds.height;
                yVelocity = 0.0f;
            }
        }
    }

    if (onGround) {
        jumpCount = 0;  // Reset jump count when the player is on the ground
        canJump = true;
    }

    shape.setPosition(x, y);
    boundDetection(1920, 1080);
}

void Player::boundDetection(int windowWidth, int windowHeight) {
    float playerWidth = shape.getSize().x;
    float playerHeight = shape.getSize().y;

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

    shape.setPosition(x, y);
}

