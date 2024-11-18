#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include "../include/Enemy.hpp"
#include <cmath>
#include <iostream>

Player::Player(float startX, float startY)
    : x(startX), y(startY), 
      prevX(startX), prevY(startY), 
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
      isJumping(false),
      isDead(false),
      respawnTimer(0.0f),
      spawnPoint(startX, startY),
      currentHealth(MAX_HEALTH),
      invulnerableTimer(0.0f)
{
    if (!walkingTexture.loadFromFile("assets/characters/player/veX_sprite_sheet.png")) {
        std::cerr << "Error loading walking texture file" << std::endl;
    }

    if (!idleTexture.loadFromFile("assets/characters/player/veX_breathe_sheet.png")) {
        std::cerr << "Error loading idle texture file" << std::endl;
    }

    if (!jumpTexture.loadFromFile("assets/characters/player/veX_jump.png")) {
        std::cerr << "Error loading jump texture file" << std::endl;
    }

    if (!deathTexture.loadFromFile("assets/characters/player/veX_death.png")) {
        std::cerr << "Error loading death texture file" << std::endl;
    }
    
    sprite.setTexture(idleTexture);
    currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentFrame);
    sprite.setPosition(x, y);
    sprite.setScale(2.0f, 2.0f);

    // Initialize health UI
    for (int i = 0; i < MAX_HEALTH; i++) {
        sf::CircleShape heart(15.f);
        heart.setFillColor(sf::Color::Red);
        heart.setPosition(50.f + (i * 40.f), 50.f);
        hearts.push_back(heart);
    }
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

void Player::update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight, Enemy& enemy) {
    if (isDead) {
        respawnTimer -= deltaTime;
        if (respawnTimer <= 0) {
            respawn();
        }
        return;
    }

    if (invulnerableTimer > 0) {
        invulnerableTimer -= deltaTime;
        // Make sprite flash when invulnerable
        sprite.setColor(sf::Color(255, 255, 255, 
            static_cast<sf::Uint8>((std::sin(invulnerableTimer * 10) + 1) * 127.5)));
    } else {
        sprite.setColor(sf::Color::White);
    }

    handleInput(deltaTime);
    applyGravity(deltaTime);
    move(deltaTime, platforms, windowWidth, windowHeight, enemy);

    animationTimer += deltaTime;
    if (animationTimer >= frameDuration) {
        currentFrameIndex = (currentFrameIndex + 1) % totalFrames;
        currentFrame.left = currentFrameIndex * frameWidth;
        sprite.setTextureRect(currentFrame);
        animationTimer = 0.0f;
    }

    sprite.setPosition(x, y);

    // Death by falling
    if (y > windowHeight + 100) {
        takeDamage();
    }
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
    drawHealthUI(window);
}

sf::FloatRect Player::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Player::collectOrb() {
    orbCount++;
}

int Player::getOrbCount() const {
    return orbCount;
}

void Player::handleInput(float deltaTime) {
    if (isDead) return;  // Don't handle input while dead

    float velocityX = 0.0f;
    bool isMoving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        if (canJump && jumpCount < maxJumps) {
            yVelocity = jumpVelocity;
            jumpCount++;
            canJump = false;
            isJumping = true;
            sprite.setTexture(jumpTexture);
            resetAnimation();
        }
    } else {
        canJump = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocityX = -speedX;
        isMoving = true;

        if (sprite.getScale().x > 0) {
            sprite.setScale(-2.0f, 2.0f);
            sprite.setOrigin(frameWidth, 0);
        }

        if (isIdle && !isJumping) {
            sprite.setTexture(walkingTexture);
            resetAnimation();
            isIdle = false;
        }
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocityX = speedX;
        isMoving = true;

        if (sprite.getScale().x < 0) {
            sprite.setScale(2.0f, 2.0f);
            sprite.setOrigin(0, 0);
        }

        if (isIdle && !isJumping) {
            sprite.setTexture(walkingTexture);
            resetAnimation();
            isIdle = false;
        }
    }

    prevX = x;
    prevY = y;
    x += velocityX * deltaTime;

    if (yVelocity >= 0) {
        isJumping = false;
    }

    if (!isMoving && !isJumping) {
        if (!isIdle) {
            sprite.setTexture(idleTexture);
            totalFrames = idleTotalFrames;
            resetAnimation();
            isIdle = true;
        }
    }

    if (!isMoving && !isJumping) {
        sprite.setTexture(idleTexture);
        isIdle = true;
    }

    if (isJumping) {
        sprite.setTexture(jumpTexture);
        resetAnimation();
    } else if (isMoving) {
        sprite.setTexture(walkingTexture);
    }
}

void Player::applyGravity(float deltaTime) {
    if (yVelocity > 0) {
        yVelocity += gravity * fallMultiplier * deltaTime;
    } else if (yVelocity < 0) {
        yVelocity += gravity * lowJumpMultiplier * deltaTime;
    } else {
        yVelocity += gravity * deltaTime;
    }

    if (yVelocity > terminalVelocity) {
        yVelocity += gravity * deltaTime;
    }
}

void Player::boundDetection(int windowWidth, int windowHeight) {
    float playerWidth = sprite.getGlobalBounds().width;
    float playerHeight = sprite.getGlobalBounds().height;

    if (x < 0) {
        x = 0;
    }
    if (x + playerWidth > windowWidth) {
        x = windowWidth - playerWidth;
    }

    if (y < 0) {
        y = 0;
        yVelocity = 0;
    }
    if (y + playerHeight > windowHeight) {
        y = windowHeight - playerHeight;
        yVelocity = 0;
        jumpCount = 0;
    }

    sprite.setPosition(x, y);
}

void Player::move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight, Enemy& enemy) {
    if (isDead) return;  // Don't move while dead

    y += yVelocity * deltaTime;

    sf::FloatRect playerBounds = sprite.getGlobalBounds();
    playerBounds.top = y;

    bool onGround = false;
    float edgeMargin = 10.0f;

    for (const auto& platform : platforms) {
        const auto& tiles = platform.getTiles();

        for (const auto& tile : tiles) {
            sf::FloatRect tileBounds = tile.getGlobalBounds();

            if (playerBounds.intersects(tileBounds)) {
                if (yVelocity > 0.0f) {
                    if ((playerBounds.top + playerBounds.height) <= tileBounds.top + edgeMargin) {
                        y = tileBounds.top - playerBounds.height;
                        yVelocity = 0.0f;
                        onGround = true;
                    }
                } else if (yVelocity < 0.0f) {
                    if (playerBounds.top >= tileBounds.top + tileBounds.height - edgeMargin) {
                        y = tileBounds.top + tileBounds.height;
                        yVelocity = 0.0f;
                    }
                }

                float playerRight = playerBounds.left + playerBounds.width;
                float playerLeft = playerBounds.left;
                float tileRight = tileBounds.left + tileBounds.width;
                float tileLeft = tileBounds.left;

                if (playerRight > tileLeft && playerLeft < tileLeft && 
                    (playerBounds.top + playerBounds.height) > tileBounds.top + edgeMargin) {
                    x = tileLeft - playerBounds.width;
                }

                if (playerLeft < tileRight && playerRight > tileRight &&
                    (playerBounds.top + playerBounds.height) > tileBounds.top + edgeMargin) {
                    x = tileRight;
                }
            }
        }
    }

    sf::FloatRect enemyBounds = enemy.getGlobalBounds();
    if (playerBounds.intersects(enemyBounds)) {
        if (!isInvulnerable() && enemy.isBossFight()) {
            takeDamage();
        }

        if (prevX < enemyBounds.left && x + playerBounds.width > enemyBounds.left) {
            x = enemyBounds.left - playerBounds.width;
        } else if (prevX > enemyBounds.left && x < enemyBounds.left + enemyBounds.width) {
            x = enemyBounds.left + enemyBounds.width;
        }

        if (prevY < enemyBounds.top && y + playerBounds.height <= enemyBounds.top + 5) {
            y = enemyBounds.top - playerBounds.height;
            yVelocity = 0.0f;
            onGround = true;
        }
    }

    if (onGround) {
        jumpCount = 0;
        canJump = true;
    }

    sprite.setPosition(x, y);
    boundDetection(windowWidth, windowHeight);
}

void Player::enemyDetection(Enemy& enemy) {
    if (isDead) return;

    if (enemy.getGlobalBounds().intersects(sprite.getGlobalBounds())) {
        if (enemy.getState() != Enemy::EnemyState::IDLE && !enemy.isBossFight()) {
            enemy.setState(Enemy::EnemyState::IDLE);  
        }
    } else {
        if (enemy.getState() != Enemy::EnemyState::PATROLLING && !enemy.isBossFight()) {
            enemy.setState(Enemy::EnemyState::PATROLLING);  
        }
    }
}

void Player::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
    prevX = newX;
    prevY = newY;
    sprite.setPosition(x, y);
}

void Player::resetState() {
    yVelocity = 0.0f;
    jumpCount = 0;
    canJump = true;
    isJumping = false;
    isIdle = true;
    resetAnimation();
}

void Player::resetAnimation() {
    currentFrameIndex = 0;
    animationTimer = 0.0f;
    currentFrame.left = 0;
    sprite.setTextureRect(currentFrame);
}

void Player::takeDamage() {
    if (invulnerableTimer > 0 || isDead) return;

    currentHealth--;
    if (currentHealth <= 0) {
        die();
    } else {
        invulnerableTimer = INVULNERABLE_DURATION;
        // Knock back player slightly
        yVelocity = jumpVelocity * 0.5f;
    }
}

void Player::die() {
    isDead = true;
    respawnTimer = RESPAWN_DELAY;
    sprite.setTexture(deathTexture);
    resetAnimation();
}

void Player::respawn() {
    isDead = false;
    currentHealth = MAX_HEALTH;
    x = spawnPoint.x;
    y = spawnPoint.y;
    yVelocity = 0.0f;
    jumpCount = 0;
    canJump = true;
    isJumping = false;
    isIdle = true;
    sprite.setTexture(idleTexture);
    resetAnimation();
    sprite.setPosition(x, y);
    invulnerableTimer = INVULNERABLE_DURATION;
}

void Player::drawHealthUI(sf::RenderWindow& window) const {
    for (int i = 0; i < MAX_HEALTH; i++) {
        // Draw empty heart
        sf::CircleShape emptyHeart = hearts[i];
        emptyHeart.setFillColor(sf::Color(100, 100, 100));
        window.draw(emptyHeart);

        // Draw filled heart if health remains
        if (i < currentHealth) {
            sf::CircleShape filledHeart = hearts[i];
            filledHeart.setFillColor(sf::Color::Red);
            window.draw(filledHeart);
        }
    }
}

void Player::setSpawnPoint(const sf::Vector2f& point) {
    spawnPoint = point;
}

bool Player::isInvulnerable() const {
    return invulnerableTimer > 0;
}

void Player::resetHealth() {
    currentHealth = MAX_HEALTH;
    isDead = false;
    invulnerableTimer = 0.0f;
}
