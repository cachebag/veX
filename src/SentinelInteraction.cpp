// SentinelInteraction.cpp

#include "../include/SentinelInteraction.hpp"
#include "../include/Enemy.hpp"
#include "../include/Player.hpp"
#include "../include/ButtonInteraction.hpp"

#include <iostream>
#include <cmath>
#include <cstdlib>

extern bool resetSentinelInteraction;

// Constructor implementation
SentinelInteraction::SentinelInteraction(sf::RenderWindow& window, sf::View& view, std::unique_ptr<Player>& player, std::unique_ptr<Enemy>& enemy)
    : window(window),
      view(view),
      player(player),
      enemy(enemy),
      questionVisible(false),
      ascent(false),
      awaitingResponse(false),
      responseComplete(false),
      awaitingFinalAnswer(false),
      sentinelHasAnswered(false),
      rng(rd()),
      dist(0, 1),
      sentinelTruth(false),
      messageDisplayDuration(sf::seconds(3.0f)),
      isCorrect(false),
      currentLevel(1),
      currentPattern(AttackPattern::DIRECT),
      patternTimer(0.0f),
      spiralAngle(0.0f),
      inBossFight(false),
      bossHealth(MAX_HEALTH),
      countdown(3),
      countdownTimer(0.0f),
      orbSpawnTimer(0.0f),
      orbSpawnInterval(0.4f),
      sentinelMoveSpeed(500.0f),
      sentinelDirection(1.0f),
      currentWave(0),
      waveComplete(false),
      waveTransitionTimer(0.0f)
{
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    healthBarBackground.setSize(sf::Vector2f(400.f, 20.f));
    healthBarBackground.setFillColor(sf::Color(100, 100, 100));
    healthBarBackground.setPosition(760.f, 50.f);

    healthBar.setSize(sf::Vector2f(400.f, 20.f));
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setPosition(760.f, 50.f);

    playerOptions.setFont(font);
    playerOptions.setCharacterSize(24);
    playerOptions.setFillColor(sf::Color::White);
}

void SentinelInteraction::resetState() {
    questionVisible = false;
    ascent = false;
    awaitingResponse = false;
    responseComplete = false;
    awaitingFinalAnswer = false;
    sentinelHasAnswered = false;
    isCorrect = false;
}

void SentinelInteraction::startLevel2Interaction() {
    // Initialize variables specific to level 2 interaction
    questionVisible = false;
    ascent = false;
    awaitingResponse = false;
    sentinelHasAnswered = false;
    responseComplete = false;
    awaitingFinalAnswer = false;
    isCorrect = false;
    currentLevel = 2;

    // Set up any level 2 specific dialogue or questions
}

void SentinelInteraction::startLevel3Interaction() {
    questionVisible = false;
    ascent = false;
    awaitingResponse = false;
    sentinelHasAnswered = false;
    responseComplete = false;
    awaitingFinalAnswer = false;
    isCorrect = false;
    currentLevel = 3;
}

void SentinelInteraction::triggerInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                             bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                             const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    if (currentLevel == 1) {
        triggerInteractionLevel1(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, playerPos, buttonInteraction, proceedToNextLevel);
    } else if (currentLevel == 2) {
        triggerInteractionLevel2(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, playerPos, buttonInteraction, proceedToNextLevel);
    } else if (currentLevel == 3) {
        triggerInteractionLevel3(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, playerPos, buttonInteraction, proceedToNextLevel);
    }
}

void SentinelInteraction::triggerInteractionLevel1(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                                   bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                                   const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    if (!enemyTriggered) return;

    if (resetSentinelInteraction) {
        resetState();
        resetSentinelInteraction = false;
    }

    if (ascent) {
        handleAscentAndCleanup(enemy, text, enemyTriggered, enemySpawned, buttonInteraction, deltaTime);
        return;
    }

    if (messageDisplayTimer.getElapsedTime() < messageDisplayDuration && messageDisplayTimer.getElapsedTime().asSeconds() != 0) {
        return;
    }

    if (isCorrect && messageDisplayTimer.getElapsedTime() >= messageDisplayDuration) {
        text.setString("");
        proceedToNextLevel = true;
        return;
    }

    handleInitialInteraction(window, text, enemyTriggered, enemyDescending, enemy, deltaTime, playerPos, buttonInteraction, proceedToNextLevel);
}

void SentinelInteraction::triggerInteractionLevel2(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                                   bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                                   const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    (void)enemySpawned;
    if (!enemyTriggered) {
        text.setString("");
        return;
    }

    if (ascent) {
        handleAscentAndCleanupLevel2(enemy, text, enemyTriggered, buttonInteraction, deltaTime);
        return;
    }

    enemy->flipSprite();

    if (resetSentinelInteraction) {
        resetState();
        resetSentinelInteraction = false;
    }

    if (enemy->getPosition().y < 200.0f) {
        return;
    }

    if (messageDisplayTimer.getElapsedTime() < messageDisplayDuration && messageDisplayTimer.getElapsedTime().asSeconds() != 0) {
        return;
    }

    if (isCorrect && messageDisplayTimer.getElapsedTime() >= messageDisplayDuration) {
        text.setString("");
        proceedToNextLevel = true;
        return;
    }

    handleInitialInteractionLevel2(window, text, enemyTriggered, enemyDescending, enemy, deltaTime, playerPos, buttonInteraction, proceedToNextLevel);
}

void SentinelInteraction::triggerInteractionLevel3(sf::RenderWindow& window, sf::Text& text,
                                                   bool& enemyTriggered, bool& enemyDescending,
                                                   bool& enemySpawned, std::unique_ptr<Enemy>& enemy,
                                                   float deltaTime, const sf::Vector2f& playerPos,
                                                   ButtonInteraction& buttonInteraction,
                                                   bool& proceedToNextLevel) {

    (void)enemySpawned;

    if (!enemyTriggered) {
        text.setString("");
        return;
    }

    if (ascent) {
        handleAscentAndCleanupLevel3(enemy, text, enemyTriggered, buttonInteraction, deltaTime);
        return;
    }

    enemy->flipSprite();

    if (resetSentinelInteraction) {
        resetState();
        resetSentinelInteraction = false;
    }

    if (enemy->getPosition().y < 200.0f) {
        return;
    }

    if (messageDisplayTimer.getElapsedTime() < messageDisplayDuration &&
        messageDisplayTimer.getElapsedTime().asSeconds() != 0) {
        return;
    }

    if (isCorrect && messageDisplayTimer.getElapsedTime() >= messageDisplayDuration) {
        text.setString("");
        proceedToNextLevel = true;
        return;
    }

    handleInitialInteractionLevel3(window, text, enemyTriggered, enemyDescending,
                                   enemy, deltaTime, playerPos, buttonInteraction,
                                   proceedToNextLevel);
}

void SentinelInteraction::handleInitialInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                                   std::unique_ptr<Enemy>& enemy, float deltaTime,
                                                   const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    // Level 1 interaction logic
    sf::FloatRect enemyBounds = enemy->getGlobalBounds();
    float targetYPosition = 600.0f;

    if (enemyDescending && enemyBounds.top < targetYPosition) {
        enemy->setPosition(enemy->getPosition().x, enemy->getPosition().y + 500.0f * deltaTime);
    } else {
        enemyDescending = false;

        if (!sentinelHasAnswered) {
            text.setString("What do you seek by summoning me?");
            text.setPosition(1400, 500);
            playerOptions.setString("Q: Nothing, go away. \nT: If Pinocchio says his \nnose will grow right now\n is he lying?");
            playerOptions.setPosition(playerPos.x, playerPos.y - 125);
            awaitingResponse = true;
        } else if (!responseComplete) {
            text.setString("Was the sentinel telling the truth? (Y/N)");

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
                checkAnswer(true, text, enemyTriggered, buttonInteraction, proceedToNextLevel);
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
                checkAnswer(false, text, enemyTriggered, buttonInteraction, proceedToNextLevel);
            }
        }
    }

    if (awaitingResponse && !sentinelHasAnswered) {
        handleQuestionResponse(text);
        window.draw(playerOptions);
    }
}

void SentinelInteraction::handleInitialInteractionLevel2(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                                         std::unique_ptr<Enemy>& enemy, float deltaTime,
                                                         const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    (void)enemyDescending;
    (void)deltaTime;

    if (!enemyTriggered) {
        text.setString("");
        return;
    }

    // Make sure enemy is at correct position before continuing
    if (enemy->getPosition().y < 200.0f) {
        return;
    }

    // Handle message display duration
    if (messageDisplayTimer.getElapsedTime() < messageDisplayDuration &&
        messageDisplayTimer.getElapsedTime().asSeconds() != 0) {
        return;
    }

    if (!sentinelHasAnswered) {
        questionVisible = true;
        text.setString("Who are you and what is it you seek?");
        text.setPosition(200, 150);
        playerOptions.setString("Q: Nothing  \nT: Did the last Sentinel lie to me? Or was he telling me the truth?");
        playerOptions.setPosition(playerPos.x, playerPos.y - 50);
        awaitingResponse = true;
    } else if (!responseComplete) {
        text.setString("Was the sentinel telling the truth? (Y/N)");

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
            checkAnswerLevel2(true, text, enemyTriggered, buttonInteraction, proceedToNextLevel);
            messageDisplayTimer.restart();
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
            checkAnswerLevel2(false, text, enemyTriggered, buttonInteraction, proceedToNextLevel);
            messageDisplayTimer.restart();
        }
    }

    if (awaitingResponse && !sentinelHasAnswered) {
        handleQuestionResponseLevel2(text);
        window.draw(playerOptions);
    }
}

void SentinelInteraction::handleInitialInteractionLevel3(sf::RenderWindow& window, sf::Text& text,
                                                         bool& enemyTriggered, bool& enemyDescending,
                                                         std::unique_ptr<Enemy>& enemy, float deltaTime,
                                                         const sf::Vector2f& playerPos,
                                                         ButtonInteraction& buttonInteraction,
                                                         bool& proceedToNextLevel) {
    (void)enemyDescending;
    (void)deltaTime;
    if (!enemyTriggered) {
        text.setString("");
        return;
    }

    if (enemy->getPosition().y < 200.0f) {
        return;
    }

    if (!sentinelHasAnswered) {
        questionVisible = true;
        text.setString("You've made it far. What is it you seek");
        text.setPosition(1200, 150);
        playerOptions.setString("Q: Please don't hurt me. \nT: Am I going to be attacked? ");
        playerOptions.setPosition(playerPos.x, playerPos.y - 50);
        awaitingResponse = true;
    } else if (!responseComplete) {
        text.setString("Was the sentinel telling the truth? (Y/N)");

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
            checkAnswerLevel3(true, text, enemyTriggered, buttonInteraction, proceedToNextLevel);
            messageDisplayTimer.restart();
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
            checkAnswerLevel3(false, text, enemyTriggered, buttonInteraction, proceedToNextLevel);
            messageDisplayTimer.restart();
        }
    }

    if (awaitingResponse && !sentinelHasAnswered) {
        handleQuestionResponseLevel3(text);
        window.draw(playerOptions);
    }
}

void SentinelInteraction::handleQuestionResponse(sf::Text& text) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        questionVisible = false;
        ascent = true;
        awaitingResponse = false;
        text.setString("");
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !sentinelHasAnswered) {
        sentinelTruth = dist(rng) == 1;

        if (dist(rng) == 1) {
            text.setString("Yes, he would be lying.");
        } else {
            text.setString("No, he would not be lying.");
        }
        messageDisplayTimer.restart();
        sentinelHasAnswered = true;
        awaitingResponse = false;
    }
}

void SentinelInteraction::handleQuestionResponseLevel2(sf::Text& text) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && !sentinelHasAnswered) {
        questionVisible = false;
        ascent = true;
        awaitingResponse = false;
        text.setString("");
        resetSentinelInteraction = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !sentinelHasAnswered) {
        if (dist(rng) == 1) {
            text.setString("Yes, the last sentinel lied to you.");
        } else {
            text.setString("No, the last sentinel was truthful.");
        }
        messageDisplayTimer.restart();
        sentinelHasAnswered = true;
        awaitingResponse = false;
    }
}

void SentinelInteraction::handleQuestionResponseLevel3(sf::Text& text) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && !sentinelHasAnswered) {
        questionVisible = false;
        ascent = true;
        awaitingResponse = false;
        text.setString("");
        resetSentinelInteraction = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !sentinelHasAnswered) {
        bool willAttack = dist(rng) == 1;

        if (willAttack) {
            text.setString("Yes... Prepare yourself...");
            inBossFight = true;
            countdown = 3;
            countdownTimer = 1.0f;
            spawnGems();
        } else {
            text.setString("You may pass. Congratulations.");
            isCorrect = true;
        }
        messageDisplayTimer.restart();
        sentinelHasAnswered = true;
        awaitingResponse = false;
    }
}

void SentinelInteraction::checkAnswer(bool playerAnswer, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    (void)playerAnswer;
    bool randomOutcome = dist(rng) == 1;

    if (randomOutcome) {
        text.setString("Correct. You may proceed.");
        ascent = true;
        isCorrect = true;
        proceedToNextLevel = true;
        messageDisplayTimer.restart();
    } else {
        text.setString("Incorrect. Try again...");
        responseComplete = true;
        sentinelHasAnswered = false;
        messageDisplayTimer.restart();
        buttonInteraction.resetPrompt();
        enemyTriggered = false;
    }
}

void SentinelInteraction::checkAnswerLevel2(bool playerAnswer, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    (void)playerAnswer;
    (void)enemyTriggered;
    bool randomOutcome = dist(rng) == 1;

    if (randomOutcome) {
        text.setString("Correct. You may proceed.");
        ascent = true;
        isCorrect = true;
        proceedToNextLevel = true;
    } else {
        text.setString("Incorrect. Try again.");
        buttonInteraction.resetPrompt();
        resetSentinelInteraction = true;
    }
}

void SentinelInteraction::checkAnswerLevel3(bool playerAnswer, sf::Text& text,
                                            bool& enemyTriggered, ButtonInteraction& buttonInteraction,
                                            bool& proceedToNextLevel) {
    (void)playerAnswer;
    (void)enemyTriggered;
    bool randomOutcome = dist(rng) == 1;

    if (randomOutcome) {
        text.setString("Correct. You have proven yourself worthy.");
        ascent = true;
        isCorrect = true;
        proceedToNextLevel = true;
    } else {
        text.setString("Incorrect. The journey continues...");
        buttonInteraction.resetPrompt();
        resetSentinelInteraction = true;
    }
}

void SentinelInteraction::startBossFight(std::unique_ptr<Enemy>& enemy) {
    (void)enemy; // Suppress unused parameter warning
    inBossFight = true;
    currentWave = 0;
    waveComplete = false;
    countdown = 3;
    countdownTimer = 1.0f;
    bossHealth = MAX_HEALTH;
    spawnWaveGems();
}

void SentinelInteraction::spawnWaveGems() {
    gems.clear();
    // Spawn the number of gems needed for current wave
    for (int i = 0; i < gemsNeededPerWave[currentWave]; ++i) {
        sf::CircleShape gem(20.f);
        gem.setFillColor(sf::Color::Yellow);
        float x = SENTINEL_MIN_X + (std::rand() % static_cast<int>(SENTINEL_MAX_X - SENTINEL_MIN_X));
        float y = 300.f + (std::rand() % 500);
        gem.setPosition(x, y);
        gems.push_back(gem);
    }
}

void SentinelInteraction::spawnGems() {
    gems.clear();
    for (int i = 0; i < 5; ++i) {
        sf::CircleShape gem(20.f);
        gem.setFillColor(sf::Color::Yellow);

        // Random position within the level bounds
        float x = SENTINEL_MIN_X + (std::rand() % static_cast<int>(SENTINEL_MAX_X - SENTINEL_MIN_X));
        float y = 300.f + (std::rand() % 500); // Adjust these values based on your level

        gem.setPosition(x, y);
        gems.push_back(gem);
    }
}

void SentinelInteraction::spawnOrbPattern(std::unique_ptr<Enemy>& enemy) {
    switch (currentPattern) {
        case AttackPattern::DIRECT:
            {
                sf::CircleShape orb(10.f);
                orb.setFillColor(sf::Color::Red);
                orb.setPosition(enemy->getPosition().x, enemy->getPosition().y + 50);
                orbs.push_back(orb);
            }
            break;

        case AttackPattern::SPIRAL:
            spawnSpiralOrbs(enemy);
            break;

        case AttackPattern::SHOTGUN:
            spawnShotgunOrbs(enemy);
            break;

        case AttackPattern::CROSS:
            spawnCrossOrbs(enemy);
            break;
    }
}

void SentinelInteraction::spawnShotgunOrbs(std::unique_ptr<Enemy>& enemy) {
    const float spreadAngle = 60.0f;
    const float angleStep = spreadAngle / (SHOTGUN_COUNT - 1);
    const float startAngle = -spreadAngle / 2;

    for (int i = 0; i < SHOTGUN_COUNT; i++) {
        float angle = startAngle + (angleStep * i);
        sf::CircleShape orb(10.f);
        orb.setFillColor(sf::Color::Yellow);
        orb.setPosition(enemy->getPosition().x, enemy->getPosition().y + 50);
        orb.setRotation(angle);
        orbs.push_back(orb);
    }
}

void SentinelInteraction::spawnCrossOrbs(std::unique_ptr<Enemy>& enemy) {
    const std::vector<sf::Vector2f> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };

    for (const auto& dir : directions) {
        sf::CircleShape orb(10.f);
        orb.setFillColor(sf::Color::Cyan);
        orb.setPosition(enemy->getPosition().x, enemy->getPosition().y + 50);
        orb.setRotation(std::atan2(dir.y, dir.x) * 180.0f / 3.14159f);
        orbs.push_back(orb);
    }
}

void SentinelInteraction::spawnSpiralOrbs(std::unique_ptr<Enemy>& enemy) {
    const float angleIncrement = 30.0f;
    spiralAngle += angleIncrement;

    sf::CircleShape orb(10.f);
    orb.setFillColor(sf::Color::Magenta);
    orb.setPosition(
        enemy->getPosition().x + std::cos(spiralAngle * 3.14159f / 180.0f) * 50.f,
        enemy->getPosition().y + std::sin(spiralAngle * 3.14159f / 180.0f) * 50.f
    );
    orbs.push_back(orb);
}

void SentinelInteraction::updateBossFight(float deltaTime, std::unique_ptr<Enemy>& enemy, const sf::Vector2f& playerPos) {
    if (!inBossFight) return;

    if (countdown > 0) {
        countdownTimer -= deltaTime;
        if (countdownTimer <= 0) {
            countdown--;
            countdownTimer = 1.0f;
        }
        return;
    }

    // Check if current wave is complete
    if (gems.empty() && !waveComplete) {
        waveComplete = true;
        waveTransitionTimer = WAVE_TRANSITION_DELAY;
        currentWave++;

        // Clear existing orbs when wave ends
        orbs.clear();

        if (currentWave >= TOTAL_WAVES) {
            // Boss fight complete
            inBossFight = false;
            isCorrect = true;
            return;
        }
    }

    // Handle wave transition
    if (waveComplete) {
        waveTransitionTimer -= deltaTime;
        if (waveTransitionTimer <= 0) {
            waveComplete = false;
            spawnWaveGems();
            countdown = 3;  // Reset countdown for next wave
            countdownTimer = 1.0f;
        }
        return;
    }

    // Update pattern timer and switch patterns
    patternTimer -= deltaTime;
    if (patternTimer <= 0) {
        patternTimer = PATTERN_SWITCH_TIME;
        int pattern = std::rand() % 4;
        currentPattern = static_cast<AttackPattern>(pattern);
    }

    // Move sentinel
    float currentX = enemy->getPosition().x;
    if (currentX >= SENTINEL_MAX_X) sentinelDirection = -1.0f;
    if (currentX <= SENTINEL_MIN_X) sentinelDirection = 1.0f;

    // Increase movement speed with each wave
    float currentMoveSpeed = sentinelMoveSpeed * (1.0f + currentWave * 0.3f);
    enemy->setPosition(
        currentX + sentinelDirection * currentMoveSpeed * deltaTime,
        SENTINEL_Y_POS
    );

    orbSpawnTimer += deltaTime;
    float currentSpawnInterval = orbSpawnInterval / (1.0f + currentWave * 0.3f);
    if (orbSpawnTimer >= currentSpawnInterval) {
        for (int i = 0; i < orbsPerPatternPerWave[currentWave]; i++) {
            spawnOrbPattern(enemy);
        }
        orbSpawnTimer = 0;
    }
    // Update orbs with wave-specific speed
    handleOrbs(deltaTime, playerPos);
}

void SentinelInteraction::handleOrbs(float deltaTime, const sf::Vector2f& playerPos) {
    for (auto it = orbs.begin(); it != orbs.end();) {
        sf::Vector2f orbPos = it->getPosition();
        sf::Vector2f direction;

        // Get direction based on orb type
        if (it->getFillColor() == sf::Color::Red) {
            direction = playerPos - orbPos;
        } else if (it->getFillColor() == sf::Color::Magenta) {
            float angle = it->getRotation() * 3.14159f / 180.0f;
            direction = sf::Vector2f(std::cos(angle), std::sin(angle));
        }
        // Handle other orb colors/types if necessary

        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length != 0) {
            direction /= length;
            // Use wave-specific speed
            float currentOrbSpeed = orbSpeedPerWave[currentWave];
            it->move(direction * currentOrbSpeed * deltaTime);
        }

        // Remove off-screen orbs
        if (orbPos.y > 1080 || orbPos.y < 0 || orbPos.x < 0 || orbPos.x > 1920) {
            it = orbs.erase(it);
        } else {
            ++it;
        }
    }
}

void SentinelInteraction::checkGemCollision(const sf::Vector2f& playerPos) {
    sf::FloatRect playerBounds(playerPos.x, playerPos.y, 64, 64); // Adjust size based on your player sprite

    for (auto it = gems.begin(); it != gems.end();) {
        sf::FloatRect gemBounds = it->getGlobalBounds();
        if (playerBounds.intersects(gemBounds)) {
            bossHealth -= 20.0f; // Each gem does 20 damage
            healthBar.setSize(sf::Vector2f((bossHealth / 100.0f) * 400.f, 20.f));
            it = gems.erase(it);

            if (bossHealth <= 0) {
                inBossFight = false;
                isCorrect = true;
                orbs.clear();
            }
        } else {
            ++it;
        }
    }
}

void SentinelInteraction::drawBossFightElements(sf::RenderWindow& window) {
    if (!inBossFight) return;

    // Draw countdown if active
    if (countdown > 0) {
        sf::Text countdownText;
        countdownText.setFont(font);
        countdownText.setCharacterSize(72);
        countdownText.setFillColor(sf::Color::White);
        countdownText.setString(waveComplete ? "Wave " + std::to_string(currentWave + 1) :
                                             std::to_string(countdown));
        countdownText.setPosition(920.f, 500.f);
        window.draw(countdownText);
    }

    // Draw wave number
    sf::Text waveText;
    waveText.setFont(font);
    waveText.setCharacterSize(30);
    waveText.setFillColor(sf::Color::White);
    waveText.setString("Wave: " + std::to_string(currentWave + 1) + "/" + std::to_string(TOTAL_WAVES));
    waveText.setPosition(50.f, 100.f);
    window.draw(waveText);

    // Draw existing elements
    window.draw(healthBarBackground);
    window.draw(healthBar);

    for (const auto& gem : gems) {
        window.draw(gem);
    }

    for (const auto& orb : orbs) {
        window.draw(orb);
    }
}

void SentinelInteraction::handleAscentAndCleanup(std::unique_ptr<Enemy>& enemy, sf::Text& text, bool& enemyTriggered,
                                                 bool& enemySpawned, ButtonInteraction& buttonInteraction, float deltaTime) {
    if (ascent) {
        float ascentSpeed = 500.0f;
        enemy->setPosition(enemy->getPosition().x, enemy->getPosition().y - ascentSpeed * deltaTime);

        if (enemy->getPosition().y + enemy->getGlobalBounds().height < 0) {
            enemy->setPosition(1600, -500);
            resetState();
            enemyTriggered = false;
            enemySpawned = false;
            text.setString("");
            buttonInteraction.resetPrompt();
            ascent = false;
            enemy->flipSprite();
        }
    }
}

void SentinelInteraction::handleAscentAndCleanupLevel2(std::unique_ptr<Enemy>& enemy, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, float deltaTime) {
    if (ascent) {
        float ascentSpeed = 500.0f;
        enemy->setPosition(enemy->getPosition().x, enemy->getPosition().y - ascentSpeed * deltaTime);

        if (enemy->getPosition().y + enemy->getGlobalBounds().height < 0) {
            enemy->setPosition(100, -500);
            resetState();
            enemyTriggered = false;
            text.setString("");
            buttonInteraction.resetPrompt();
            ascent = false;
            resetSentinelInteraction = true;
        }
    }
}

void SentinelInteraction::handleAscentAndCleanupLevel3(std::unique_ptr<Enemy>& enemy,
                                                       sf::Text& text, bool& enemyTriggered,
                                                       ButtonInteraction& buttonInteraction,
                                                       float deltaTime) {
    if (ascent) {
        float ascentSpeed = 500.0f;
        enemy->setPosition(enemy->getPosition().x,
                           enemy->getPosition().y - ascentSpeed * deltaTime);

        if (enemy->getPosition().y + enemy->getGlobalBounds().height < 0) {
            enemy->setPosition(1600, -500);
            resetState();
            enemyTriggered = false;
            text.setString("");
            buttonInteraction.resetPrompt();
            ascent = false;
            resetSentinelInteraction = true;
        }
    }
}

bool SentinelInteraction::isAscending() const {
    return ascent;
}
