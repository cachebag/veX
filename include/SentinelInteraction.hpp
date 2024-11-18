#ifndef SENTINEL_INTERACTION_HPP
#define SENTINEL_INTERACTION_HPP

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <map>
#include "ButtonInteraction.hpp"
#include "Enemy.hpp"
#include "Player.hpp"       // Include Player class
#include "Platform.hpp"     // Include Platform class

class SentinelInteraction {
public:
    // Constructor accepting references to window, view, player, and enemy
    SentinelInteraction(sf::RenderWindow& window, sf::View& view, std::unique_ptr<Player>& player, std::unique_ptr<Enemy>& enemy);
    void resetState();
    void startLevel2Interaction();
    void startLevel3Interaction();
    void triggerInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                            bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                            const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    bool isAscending() const;

    // Level-specific interaction methods
    void triggerInteractionLevel1(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void triggerInteractionLevel2(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void triggerInteractionLevel3(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered,
                                  bool& enemyDescending, bool& enemySpawned, std::unique_ptr<Enemy>& enemy,
                                  float deltaTime, const sf::Vector2f& playerPos,
                                  ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);

    // Boss fight methods
    bool isInBossFight() const { return inBossFight; }
    void startBossFight(std::unique_ptr<Enemy>& enemy);
    void updateBossFight(float deltaTime, std::unique_ptr<Enemy>& enemy, const sf::Vector2f& playerPos);
    void drawBossFightElements(sf::RenderWindow& window);
    const std::vector<sf::CircleShape>& getOrbs() const { return orbs; }
    void checkGemCollision(const sf::Vector2f& playerPos);

private:
    // Basic variables
    sf::Font font;
    sf::Text playerOptions;
    bool questionVisible;
    bool ascent;
    bool awaitingResponse;
    bool responseComplete;
    bool awaitingFinalAnswer;
    bool sentinelHasAnswered;
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;
    bool sentinelTruth;
    sf::Clock messageDisplayTimer;
    sf::Time messageDisplayDuration;
    bool isCorrect;
    int currentLevel;
    static constexpr float MAX_HEALTH = 100.0f;

    // Attack pattern enum and variables
    enum class AttackPattern {
        DIRECT,
        SPIRAL,
        SHOTGUN,
        CROSS
    };
    AttackPattern currentPattern;
    float patternTimer;
    float spiralAngle;

    // Boss fight variables
    bool inBossFight;
    float bossHealth;
    std::vector<sf::CircleShape> orbs;
    std::vector<sf::CircleShape> gems;
    sf::RectangleShape healthBar;
    sf::RectangleShape healthBarBackground;
    int countdown;
    float countdownTimer;
    float orbSpawnTimer;
    float orbSpawnInterval;
    float sentinelMoveSpeed;
    float sentinelDirection;

    // Wave system variables
    int currentWave;
    bool waveComplete;
    float waveTransitionTimer;
    static constexpr int TOTAL_WAVES = 3;
    static constexpr float WAVE_TRANSITION_DELAY = 3.0f;
    static constexpr float PATTERN_SWITCH_TIME = 5.0f;
    static constexpr int SHOTGUN_COUNT = 5;
    static constexpr float SENTINEL_MIN_X = 400.0f;
    static constexpr float SENTINEL_MAX_X = 1500.0f;
    static constexpr float SENTINEL_Y_POS = 100.0f;

    int gemsNeededPerWave[3] = {3, 4, 5};
    float orbSpeedPerWave[3] = {600.0f, 800.0f, 1000.0f};
    int orbsPerPatternPerWave[3] = {5, 7, 9};

    // Level loading variables
    std::vector<sf::Vector2f> tilePositions;
    std::vector<Platform> platforms;
    std::map<std::string, sf::Texture> textureMap;

    // References to external objects
    std::unique_ptr<Enemy>& enemy;
    std::unique_ptr<Player>& player;
    sf::RenderWindow& window;
    sf::View& view;

    // Existing interaction methods
    void handleInitialInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleQuestionResponse(sf::Text& text);
    void checkAnswer(bool playerAnswer, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleAscentAndCleanup(std::unique_ptr<Enemy>& enemy, sf::Text& text, bool& enemyTriggered,
                                bool& enemySpawned, ButtonInteraction& buttonInteraction, float deltaTime);

    void handleInitialInteractionLevel2(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                        std::unique_ptr<Enemy>& enemy, float deltaTime,
                                        const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleQuestionResponseLevel2(sf::Text& text);
    void checkAnswerLevel2(bool playerAnswer, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleAscentAndCleanupLevel2(std::unique_ptr<Enemy>& enemy, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, float deltaTime);

    void handleInitialInteractionLevel3(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered,
                                        bool& enemyDescending, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                        const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction,
                                        bool& proceedToNextLevel);
    void handleQuestionResponseLevel3(sf::Text& text);
    void checkAnswerLevel3(bool playerAnswer, sf::Text& text, bool& enemyTriggered,
                           ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleAscentAndCleanupLevel3(std::unique_ptr<Enemy>& enemy, sf::Text& text,
                                      bool& enemyTriggered, ButtonInteraction& buttonInteraction,
                                      float deltaTime);

    // Boss fight private methods
    void spawnGems();
    void handleOrbs(float deltaTime, const sf::Vector2f& playerPos);
    void spawnOrbPattern(std::unique_ptr<Enemy>& enemy);
    void spawnSpiralOrbs(std::unique_ptr<Enemy>& enemy);
    void spawnShotgunOrbs(std::unique_ptr<Enemy>& enemy);
    void spawnCrossOrbs(std::unique_ptr<Enemy>& enemy);

    // Wave-specific methods
    void startNextWave();
    void updateWave();
    void spawnWaveGems();
    bool isWaveComplete() const;
};

#endif // SENTINEL_INTERACTION_HPP

