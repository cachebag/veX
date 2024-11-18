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
    (void)buttonInteraction;
    (void)proceedToNextLevel;

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
        playerOptions.setString("Q: Please don't hurt me. \nT: Are you going to attack me now?");
        playerOptions.setPosition(playerPos.x, playerPos.y - 50);
        awaitingResponse = true;
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
        text.setString("Prepare yourself...");
        text.setPosition(300, 600);
        inBossFight = true;
        countdown = 3;
        countdownTimer = 1.0f;
        spawnWaveGems();
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
    
    // Spawn fewer gems that are easier to collect
    int gemsToSpawn = 3 + currentWave; // Starts with 3 gems, adds one per wave
    
    for (int i = 0; i < gemsToSpawn; ++i) {
        sf::CircleShape gem(25.f); // Larger gems for easier collection
        gem.setFillColor(sf::Color::Yellow);
        
        // More predictable gem placement
        float angle = (360.0f / gemsToSpawn) * i;
        float radius = 300.f; // Fixed radius from center
        float centerX = (SENTINEL_MIN_X + SENTINEL_MAX_X) / 2;
        float centerY = 500.f;
        
        float x = centerX + radius * std::cos(angle * 3.14159f / 180.0f);
        float y = centerY + radius * std::sin(angle * 3.14159f / 180.0f);
        
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
        case AttackPattern::DIRECT: {
            Orb orb(10.f, AttackPattern::DIRECT);
            orb.shape.setFillColor(sf::Color::Red);
            orb.shape.setPosition(enemy->getPosition().x, enemy->getPosition().y + 50);
            orbs.push_back(orb);
            break;
        }
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
    const float startAngle = 90.0f - (spreadAngle / 2); // Center at 90 degrees (downward)

    for (int i = 0; i < SHOTGUN_COUNT; i++) {
        float angle = startAngle + (angleStep * i);
        Orb orb(10.f, AttackPattern::SHOTGUN);
        orb.shape.setFillColor(sf::Color::Yellow);
        orb.shape.setPosition(enemy->getPosition().x, enemy->getPosition().y + 50);
        orb.angle = angle;
        orbs.push_back(orb);
    }
}

void SentinelInteraction::spawnCrossOrbs(std::unique_ptr<Enemy>& enemy) {
    const std::vector<float> angles = {0, 45, 90, 135, 180, 225, 270, 315};
    
    for (float angle : angles) {
        Orb orb(10.f, AttackPattern::CROSS);
        orb.shape.setFillColor(sf::Color::Cyan);
        orb.shape.setPosition(enemy->getPosition().x, enemy->getPosition().y + 50);
        orb.angle = angle;
        orbs.push_back(orb);
    }
}

void SentinelInteraction::spawnSpiralOrbs(std::unique_ptr<Enemy>& enemy) {
    Orb orb(10.f, AttackPattern::SPIRAL);
    orb.shape.setFillColor(sf::Color::Magenta);
    orb.shape.setPosition(enemy->getPosition().x, enemy->getPosition().y + 50);
    orb.angle = spiralAngle;
    orbs.push_back(orb);
    
    spiralAngle += 80.0f;
    if (spiralAngle >= 360.0f) {
        spiralAngle = 0.0f;
    }
}


void SentinelInteraction::updateBossFight(float deltaTime, std::unique_ptr<Enemy>& enemy, const sf::Vector2f& playerPos) {
    if (showVictoryScreen) {
        updateVictoryScreen(deltaTime);
        return;
    }
    
    if (!inBossFight) return;

    // Countdown logic with clearer visual feedback
    if (countdown > 0) {
        canPlayerMove = false;
        countdownTimer -= deltaTime;
        if (countdownTimer <= 0) {
            countdown--;
            countdownTimer = 1.0f;
            if (countdown == 0) {
                canPlayerMove = true;
                spawnWaveGems();
            }
        }
        return;
    }
    
    // Simplified wave completion conditions
    if (!waveComplete && orbsCollected >= gemsNeededPerWave[currentWave]) {
        waveComplete = true;
        waveTransitionTimer = WAVE_TRANSITION_DELAY;
        
        // Check for final victory first
        if (currentWave >= TOTAL_WAVES - 1 && orbsCollected >=gemsNeededPerWave[currentWave]) {  // -1 because waves are 0-based
            bossHealth = 0;
            inBossFight = false;
            isCorrect = true;
            showVictoryScreen = true;
            createVictoryParticles();
            return;  // Exit immediately after triggering victory
        }
        
        // If not final wave, proceed to next wave
        currentWave++;
        orbsCollected = 0;
        canPlayerMove = false;
        orbs.clear();
        gems.clear();
        bossHealth = MAX_HEALTH * (1.0f + (currentWave * 0.3f)); // Reduced health scaling
        healthBar.setSize(sf::Vector2f(400.f, 20.f));
    }

    // Wave transition period
    if (waveComplete) {
        waveTransitionTimer -= deltaTime;
        if (waveTransitionTimer <= 0) {
            waveComplete = false;
            spawnWaveGems();
            countdown = 3;
            countdownTimer = 1.0f;
        }
        return;
    }

    // Slower pattern switching
    patternTimer -= deltaTime;
    if (patternTimer <= 0) {
        patternTimer = PATTERN_SWITCH_TIME * 1.5f; // More time between pattern changes
        int pattern = std::rand() % 4;
        currentPattern = static_cast<AttackPattern>(pattern);
    }

    // Slower sentinel movement
    float currentX = enemy->getPosition().x;
    if (currentX >= SENTINEL_MAX_X) sentinelDirection = -1.0f;
    if (currentX <= SENTINEL_MIN_X) sentinelDirection = 1.0f;

    float currentMoveSpeed = sentinelMoveSpeed * (1.0f + currentWave * 0.2f); // Reduced speed scaling
    enemy->setPosition(
        currentX + sentinelDirection * currentMoveSpeed * deltaTime,
        SENTINEL_Y_POS
    );

    // Reduced orb spawning frequency
    orbSpawnTimer += deltaTime;
    float currentSpawnInterval = orbSpawnInterval * (1.0f + (currentWave * 0.1f)); // Slower orbs at higher waves
    if (orbSpawnTimer >= currentSpawnInterval) {
        spawnOrbPattern(enemy);
        orbSpawnTimer = 0;
    }
    
    handleOrbs(deltaTime, playerPos);
}

void SentinelInteraction::createVictoryParticles() {
    particles.clear();
    
    // Create celebration particles
    for (int i = 0; i < 100; ++i) {
        Particle particle;
        particle.shape.setRadius(std::rand() % 5 + 2.0f);
        
        // Random colors for particles
        sf::Color particleColor;
        switch (std::rand() % 4) {
            case 0: particleColor = sf::Color::Yellow; break;
            case 1: particleColor = sf::Color(255, 215, 0); break;  // Gold
            case 2: particleColor = sf::Color(255, 140, 0); break;  // Dark Orange
            case 3: particleColor = sf::Color::White; break;
        }
        
        particle.shape.setFillColor(particleColor);
        particle.shape.setPosition(960.f, 540.f);  // Screen center
        
        // Random velocity
        float angle = (std::rand() % 360) * 3.14159f / 180.f;
        float speed = std::rand() % 300 + 200.0f;
        particle.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        
        particle.lifetime = std::rand() % 3 + 2.0f;  // 2-5 seconds lifetime
        particles.push_back(particle);
    }
}

void SentinelInteraction::updateVictoryScreen(float deltaTime) {
    // Fade in effect
    if (victoryScreenAlpha < 255.0f) {
        victoryScreenAlpha += deltaTime * 200.0f;
        if (victoryScreenAlpha > 255.0f) victoryScreenAlpha = 255.0f;
    }
    
    // Update particles
    for (auto it = particles.begin(); it != particles.end();) {
        it->lifetime -= deltaTime;
        
        if (it->lifetime <= 0) {
            it = particles.erase(it);
            continue;
        }
        
        // Apply gravity and update position
        it->velocity.y += 100.0f * deltaTime;  // Gravity
        it->shape.move(it->velocity * deltaTime);
        
        // Fade out particles as they age
        sf::Color color = it->shape.getFillColor();
        color.a = static_cast<sf::Uint8>((it->lifetime / 3.0f) * 255);
        it->shape.setFillColor(color);
        
        ++it;
    }
    
    // Spawn new particles occasionally
    victoryScreenTimer += deltaTime;
    if (victoryScreenTimer >= 0.1f) {
        victoryScreenTimer = 0.0f;
        if (particles.size() < 50) {  // Maintain some particles
            createVictoryParticles();
        }
    }
}

void SentinelInteraction::drawVictoryScreen(sf::RenderWindow& window) {
    if (!showVictoryScreen) return;

    // Draw semi-transparent background
    sf::RectangleShape background(sf::Vector2f(1920.f, 1080.f));
    background.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(victoryScreenAlpha * 0.7f)));
    window.draw(background);
    
    // Draw victory text
    sf::Text victoryText;
    victoryText.setFont(font);
    victoryText.setString("VICTORY!");
    victoryText.setCharacterSize(100);
    victoryText.setFillColor(sf::Color(255, 215, 0, static_cast<sf::Uint8>(victoryScreenAlpha)));
    
    // Center the text
    sf::FloatRect textBounds = victoryText.getLocalBounds();
    victoryText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    victoryText.setPosition(960.f, 440.f);
    
    // Add subtle pulsing effect
    float scale = 1.0f + std::sin(victoryScreenTimer * 2.0f) * 0.05f;
    victoryText.setScale(scale, scale);
    
    window.draw(victoryText);
    
    // Draw congratulatory message
    sf::Text congratsText;
    congratsText.setFont(font);
    congratsText.setString("You have defeated the Sentinel!");
    congratsText.setCharacterSize(40);
    congratsText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(victoryScreenAlpha)));
    
    // Center the congratulatory text
    textBounds = congratsText.getLocalBounds();
    congratsText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    congratsText.setPosition(960.f, 540.f);
    
    window.draw(congratsText);
    
    // Draw all particles
    for (const auto& particle : particles) {
        window.draw(particle.shape);
    }
}

void SentinelInteraction::handleOrbs(float deltaTime, const sf::Vector2f& playerPos) {
    for (auto it = orbs.begin(); it != orbs.end();) {
        sf::Vector2f orbPos = it->shape.getPosition();
        float speed = orbSpeedPerWave[currentWave] * 0.7f; // Reduced orb speed

        // Pattern-specific behavior
        switch (it->patternType) {
            case AttackPattern::DIRECT: {
                // Slower homing speed
                it->direction = playerPos - orbPos;
                float length = std::sqrt(it->direction.x * it->direction.x + it->direction.y * it->direction.y);
                if (length != 0) {
                    it->direction /= length;
                }
                speed *= 0.8f; // Even slower for homing orbs
                break;
            }
            case AttackPattern::SHOTGUN:
            case AttackPattern::CROSS: {
                float angle = it->angle * 3.14159f / 180.0f;
                it->direction = sf::Vector2f(std::cos(angle), std::sin(angle));
                break;
            }
            case AttackPattern::SPIRAL: {
                it->angle += deltaTime * 270.0f; // Slower rotation
                float angle = it->angle * 3.14159f / 180.0f;
                it->direction = sf::Vector2f(std::cos(angle), std::sin(angle));
                speed *= 0.9f;
                break;
            }
        }

        sf::Vector2f newPos = orbPos + it->direction * speed * deltaTime;
        
        // Bounds checking and collision
        sf::FloatRect newOrbBounds = it->shape.getGlobalBounds();
        newOrbBounds.left = newPos.x;
        newOrbBounds.top = newPos.y;

        bool collided = false;
        for (const auto& platform : platforms) {
            for (const auto& tile : platform.getTiles()) {
                if (newOrbBounds.intersects(tile.getGlobalBounds())) {
                    collided = true;
                    break;
                }
            }
            if (collided) break;
        }

        // Remove orbs that are out of bounds or collided
        if (collided || 
            newPos.y > 1080 || 
            newPos.y < 0 || 
            newPos.x < 0 || 
            newPos.x > 1920) {
            it = orbs.erase(it);
        } else {
            it->shape.setPosition(newPos);
            ++it;
        }
    }
}

void SentinelInteraction::checkGemCollision(const sf::Vector2f& playerPos) {
    if (!canPlayerMove) return;

    sf::FloatRect playerBounds(playerPos.x, playerPos.y, 64, 64);

    for (auto it = gems.begin(); it != gems.end();) {
        sf::FloatRect gemBounds = it->getGlobalBounds();
        if (playerBounds.intersects(gemBounds)) {
            float damage = 20.0f / (1.0f + currentWave * 0.5f);
            bossHealth -= damage;
            if (bossHealth < 0) bossHealth = 0; 
            healthBar.setSize(sf::Vector2f((bossHealth / (MAX_HEALTH * (1.0f + currentWave * 0.5f))) * 400.f, 20.f));
            orbsCollected++;
            it = gems.erase(it);

            if (orbsCollected >= gemsNeededPerWave[currentWave]) {
                if (currentWave >= TOTAL_WAVES - 1) {
                    bossHealth = 0;
                }
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
        window.draw(orb.shape); 
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

// Add this new method to check if player can move
/*bool SentinelInteraction::canMove() const {
    return canPlayerMove;
}*/

bool SentinelInteraction::isAscending() const {
    return ascent;
}

