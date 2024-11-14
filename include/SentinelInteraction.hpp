// SentinelInteraction.hpp

#ifndef SENTINEL_INTERACTION_HPP
#define SENTINEL_INTERACTION_HPP

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include "ButtonInteraction.hpp"
#include "Enemy.hpp"

class SentinelInteraction {
public:
    SentinelInteraction();
    void resetState();
    void startLevel2Interaction();
    void triggerInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                            bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                            const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    bool isAscending() const;

    // Level-specific interaction methods moved to public
    void triggerInteractionLevel1(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void triggerInteractionLevel2(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, bool& proceedToNextLevel);

private:
    // Member variables
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
    int currentLevel; // Added member variable

    // Level 1 interaction methods
    void handleInitialInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                  std::unique_ptr<Enemy>& enemy, float deltaTime,
                                  const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleQuestionResponse(sf::Text& text);
    void checkAnswer(bool playerAnswer, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel);
    void handleAscentAndCleanup(std::unique_ptr<Enemy>& enemy, sf::Text& text, bool& enemyTriggered,
                                bool& enemySpawned, ButtonInteraction& buttonInteraction, float deltaTime);

    // Level 2 interaction methods
    void handleInitialInteractionLevel2(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                        std::unique_ptr<Enemy>& enemy, float deltaTime,
                                        const sf::Vector2f& playerPos, bool& proceedToNextLevel);
    void handleQuestionResponseLevel2(sf::Text& text);
    void checkAnswerLevel2(bool playerAnswer, sf::Text& text, bool& enemyTriggered, bool& proceedToNextLevel);
    void handleAscentAndCleanupLevel2(std::unique_ptr<Enemy>& enemy, sf::Text& text, bool& enemyTriggered, float deltaTime);
};

#endif // SENTINEL_INTERACTION_HPP

