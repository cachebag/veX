#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp" // Include Platform class for collision detection

class Player {
public:
    Player(float startX = 0.0f, float startY = 500.0f);

    void update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getGlobalBounds() const;

    void collectOrb();
    int getOrbCount() const;

private:
    float x, y; // Player position
    float yVelocity; // Player vertical velocity
    const float gravity; // Gravity applied to the player
    const float terminalVelocity; // Maximum falling speed
    const float speedX; // Horizontal movement speed
    const float jumpVelocity; // Jump strength

    int jumpCount; // Tracks how many jumps have been used
    const int maxJumps; // Maximum number of jumps (e.g., for double jump)
    const float fallMultiplier; // Speed multiplier when falling
    const float lowJumpMultiplier; // Jump height control

    int orbCount; // Number of orbs collected by the player

    sf::Texture walkingTexture; // Texture for walking animation
    sf::Texture idleTexture; // Texture for idle animation
    sf::Texture jumpTexture; // Texture for jumping
    sf::Sprite sprite; // Sprite to represent the player
    sf::IntRect currentFrame; // Current animation frame (position and size)

    int currentFrameIndex; // Current frame index
    float animationTimer; // Time elapsed since last frame change
    float frameDuration; // Time between each frame change
    bool isIdle; // Indicates if the player is idle or moving
    bool canJump;
    bool isJumping;

    const int frameWidth = 32; // Width of each frame in the sprite sheet
    const int frameHeight = 32; // Height of each frame in the sprite sheet
    const int totalFrames = 3; // Number of frames in the animation

    void handleInput(float deltaTime);
    void applyGravity(float deltaTime);
    void move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight);
    void boundDetection(int windowWidth, int windowHeight);
    void resetAnimation();
};

