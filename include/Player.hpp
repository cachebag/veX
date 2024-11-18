#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp"
#include "Enemy.hpp"

class Player {
public:
    // Constructor
    Player(float startX = 0.0f, float startY = 500.0f);

    // Core game loop methods
    void update(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight, Enemy& enemy);
    void draw(sf::RenderWindow& window) const;
    
    // Collision and bounds
    sf::FloatRect getGlobalBounds() const;
    
    // Orb collection
    void collectOrb();
    int getOrbCount() const;
    
    // Position getters and setters
    sf::Vector2f getPosition() const;
    void setPosition(float newX, float newY);
    
    // State management
    void resetState();
    
    // Health and death system
    void takeDamage();
    void die();
    void respawn();
    bool isPlayerDead() const { return isDead; }
    void setSpawnPoint(const sf::Vector2f& point); 
    bool isInvulnerable() const;
    void resetHealth();
    int getHealth() const { return currentHealth; }

private:
    // Position and movement variables
    float x, y;
    float prevX, prevY;
    float yVelocity;
    const float gravity;
    const float terminalVelocity;
    const float speedX;
    const float jumpVelocity;
    int jumpCount;
    const int maxJumps;
    const float fallMultiplier;
    const float lowJumpMultiplier;
    int orbCount;

    // Sprite and animation variables
    sf::Texture walkingTexture;
    sf::Texture idleTexture;
    sf::Texture jumpTexture;
    sf::Texture deathTexture;
    sf::Sprite sprite;
    sf::IntRect currentFrame;
    int currentFrameIndex;
    float animationTimer;
    float frameDuration;
    bool isIdle;
    bool canJump;
    bool isJumping;
    const int frameWidth = 32;
    const int frameHeight = 32;
    int totalFrames = 3;
    const int idleTotalFrames = 3;

    // Health and death system variables
    bool isDead;
    float respawnTimer;
    sf::Vector2f spawnPoint;
    int currentHealth;
    float invulnerableTimer;
    std::vector<sf::CircleShape> hearts;

    // Constants for health and death system
    static const int MAX_HEALTH = 3;
    static const float RESPAWN_DELAY;
    static const float INVULNERABLE_DURATION;

    // Movement and physics methods
    void handleInput(float deltaTime);
    void applyGravity(float deltaTime);
    void move(float deltaTime, const std::vector<Platform>& platforms, int windowWidth, int windowHeight, Enemy& enemy);
    void boundDetection(int windowWidth, int windowHeight);
    void enemyDetection(Enemy& enemy);

    // Animation methods
    void resetAnimation();

    // UI methods
    void drawHealthUI(sf::RenderWindow& window) const;
};

// Static constant definitions
inline constexpr float Player::RESPAWN_DELAY = 1.0f;
inline constexpr float Player::INVULNERABLE_DURATION = 1.5f;
