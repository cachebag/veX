#ifndef SENTINEL_INTERACTION_HPP
#define SENTINEL_INTERACTION_HPP

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <map>
#include <array>
#include "ButtonInteraction.hpp"
#include "Enemy.hpp"
#include "Platform.hpp"

class Player;

class SentinelInteraction {
public:
    SentinelInteraction(sf::RenderWindow& window, sf::View& view, std::unique_ptr<Player>& player, std::unique_ptr<Enemy>& enemy);
    void resetState();
    void startLevel2Interaction();
    void startLevel3Interaction();
    void triggerInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                            bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                            const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    bool isAscending() const;
    bool isInBossFight() const { return inBossFight; }
    bool canMove() const { return canPlayerMove; }
    void startBossFight(std::unique_ptr<Enemy>& enemy);
    void updateBossFight(float deltaTime, std::unique_ptr<Enemy>& enemy, const sf::Vector2f& playerPos);
    void drawBossFightElements(sf::RenderWindow& window);
    void createVictoryParticles();
    void updateVictoryScreen(float deltaTime);
    void drawVictoryScreen(sf::RenderWindow& window);
    
    const std::vector<sf::CircleShape>& getOrbs() const {
        static std::vector<sf::CircleShape> orbShapes;
        orbShapes.clear();
        for (const auto& orb : orbs) {
            orbShapes.push_back(orb.shape);
        }
        return orbShapes;
    }

    void setCurrentPlatforms(const std::vector<Platform>& newPlatforms) {
        platforms = newPlatforms;
    }
    
    bool isVictorious() const { return showVictoryScreen; }
    void checkGemCollision(const sf::Vector2f& playerPos);

    void triggerInteractionLevel1(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void triggerInteractionLevel2(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void triggerInteractionLevel3(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);

private:
    static constexpr float MAX_HEALTH = 100.0f;
    static constexpr float PATTERN_SWITCH_TIME = 5.0f;
    static constexpr int SHOTGUN_COUNT = 3;
    static constexpr float SENTINEL_MIN_X = 400.0f;
    static constexpr float SENTINEL_MAX_X = 1500.0f;
    static constexpr float SENTINEL_Y_POS = 100.0f;
    static constexpr int TOTAL_WAVES = 3;
    static constexpr float WAVE_TRANSITION_DELAY = 3.0f;
    static constexpr int ORBS_PER_WAVE = 5;
    static constexpr int COLLECTIONS_PER_WAVE = 2;

    enum class AttackPattern {
        DIRECT,
        SPIRAL,
        SHOTGUN,
        CROSS
    };

    struct Orb {
        sf::CircleShape shape;
        sf::Vector2f direction{0.f, 0.f};
        AttackPattern patternType;
        float angle{0.0f};
        
        Orb(float radius, AttackPattern t) 
            : shape(radius), patternType(t) {}
    };

    struct Particle {
        sf::CircleShape shape;
        sf::Vector2f velocity;
        float lifetime;
    };

    sf::RenderWindow& window;
    sf::View& view;
    std::unique_ptr<Player>& player;
    std::unique_ptr<Enemy>& enemy;
    
    sf::Font font;
    sf::Text playerOptions;
    bool questionVisible{false};
    bool ascent{false};
    bool awaitingResponse{false};
    bool responseComplete{false};
    bool awaitingFinalAnswer{false};
    bool sentinelHasAnswered{false};
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;
    bool sentinelTruth{false};
    sf::Clock messageDisplayTimer;
    sf::Time messageDisplayDuration;
    bool isCorrect{false};
    int currentLevel{1};

    // Victory screen variables
    bool showVictoryScreen{false};
    float victoryScreenAlpha{0.0f};
    float victoryScreenTimer{0.0f};
    std::vector<Particle> particles;

    AttackPattern currentPattern{AttackPattern::DIRECT};
    float patternTimer{0.0f};
    float spiralAngle{0.0f};

    bool inBossFight{false};
    float bossHealth{MAX_HEALTH};
    std::vector<Orb> orbs;
    std::vector<sf::CircleShape> gems;
    sf::RectangleShape healthBar;
    sf::RectangleShape healthBarBackground;
    int countdown{3};
    float countdownTimer{0.0f};
    float orbSpawnTimer{0.0f};
    float orbSpawnInterval{0.4f};
    float sentinelMoveSpeed{500.0f};
    float sentinelDirection{1.0f};

    int currentWave{0};
    bool waveComplete{false};
    float waveTransitionTimer{0.0f};
    
    // Wave control variables
    int orbsCollected{0};
    int currentCollection{0};
    bool canPlayerMove{true};
    bool wavePaused{false};
    float waveStartTimer{3.0f};

    std::array<int, TOTAL_WAVES> gemsNeededPerWave{{3, 4, 5}};
    std::array<float, TOTAL_WAVES> orbSpeedPerWave{{600.0f, 800.0f, 1000.0f}};
    std::array<int, TOTAL_WAVES> orbsPerPatternPerWave{{1, 3, 5}};

    std::vector<sf::Vector2f> tilePositions;
    std::vector<Platform> platforms;
    std::map<AssetType, sf::Texture> textureMap;

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

    void handleInitialInteractionLevel3(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                        std::unique_ptr<Enemy>& enemy, float deltaTime,
                                        const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleQuestionResponseLevel3(sf::Text& text);
    void checkAnswerLevel3(bool playerAnswer, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleAscentAndCleanupLevel3(std::unique_ptr<Enemy>& enemy, sf::Text& text, bool& enemyTriggered,
                                      ButtonInteraction& buttonInteraction, float deltaTime);

    void spawnGems();
    void handleOrbs(float deltaTime, const sf::Vector2f& playerPos);
    void spawnOrbPattern(std::unique_ptr<Enemy>& enemy);
    void spawnSpiralOrbs(std::unique_ptr<Enemy>& enemy);
    void spawnShotgunOrbs(std::unique_ptr<Enemy>& enemy);
    void spawnCrossOrbs(std::unique_ptr<Enemy>& enemy);
    void spawnWaveGems();
};

#endif
