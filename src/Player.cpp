// Player.cpp
#include "../include/Player.hpp"
#include "../include/Platform.hpp"

Player::Player(float startX, float startY) 
    : x(startX), y(startY), yVelocity(0.0f), gravity(150.0f), terminalVelocity(10.0f), speedX(400.0f), jumpVelocity(-30.0f) {
    shape.setSize(sf::Vector2f(50.0f, 50.0f));
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(x, y);
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

void Player::handleInput(float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        x -= speedX * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        x += speedX * deltaTime;
    }
    shape.setPosition(x, y);
}

void Player::applyGravity(float deltaTime) {
    yVelocity += gravity * deltaTime;
    if (yVelocity > terminalVelocity) {
        yVelocity += gravity * deltaTime;
    }
}

void Player::move(float deltaTime, const std::vector<Platform>& platforms) {
    y += yVelocity;
    sf::FloatRect playerBounds = shape.getGlobalBounds();
    playerBounds.top = y;

    bool onGround = false;

    for (const auto& platform : platforms) {
        if (playerBounds.intersects(platform.getBounds())) {
            sf::FloatRect platformBounds = platform.getBounds();
            if (yVelocity > 0.0f && (playerBounds.top + playerBounds.height) > platformBounds.top) {
                y = platformBounds.top - playerBounds.height;
                yVelocity = 0.0f;
                onGround = true;
            }
        }
    }

    shape.setPosition(x, y);

    if (onGround && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        yVelocity = jumpVelocity;
    }
}

