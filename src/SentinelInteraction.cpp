// SentinelInteraction.cpp

#include "../include/SentinelInteraction.hpp"
#include "../include/Enemy.hpp"

extern bool resetSentinelInteraction;

SentinelInteraction::SentinelInteraction()
    : questionVisible(false), ascent(false), awaitingResponse(false), responseComplete(false),
      awaitingFinalAnswer(false), sentinelHasAnswered(false), rng(rd()), dist(0, 1),
      messageDisplayDuration(sf::seconds(3.0f)), isCorrect(false), currentLevel(1) {

    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Failed to load font\n";
    }

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
    questionVisible = true;
    ascent = false;
    awaitingResponse = true;
    sentinelHasAnswered = false;
    responseComplete = false;
    awaitingFinalAnswer = false;
    isCorrect = false;
    currentLevel = 2;

    // Set up any level 2 specific dialogue or questions
}

void SentinelInteraction::triggerInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                             bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                             const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    if (currentLevel == 1) {
        triggerInteractionLevel1(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, playerPos, buttonInteraction, proceedToNextLevel);
    } else if (currentLevel == 2) {
        triggerInteractionLevel2(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, playerPos, proceedToNextLevel);
    }
}

void SentinelInteraction::triggerInteractionLevel1(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                                   bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                                   const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    // Original level 1 interaction code
    // (Same as in your original code)
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
                                                   const sf::Vector2f& playerPos, bool& proceedToNextLevel) {
    if (!enemyTriggered) return;  // Early exit if enemy is not triggered

    enemy->flipSprite();  // Flip sprite at the start of the interaction
    
    if (resetSentinelInteraction) {
        resetState();
        resetSentinelInteraction = false;
    }

    if (ascent) {
        handleAscentAndCleanupLevel2(enemy, text, enemyTriggered, deltaTime);
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

    handleInitialInteractionLevel2(window, text, enemyTriggered, enemyDescending, enemy, deltaTime, playerPos, proceedToNextLevel);
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
            playerOptions.setString("Q: Nothing, go away \nT: Will your next response be a lie?");
            playerOptions.setPosition(playerPos.x, playerPos.y - 50);
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
                                                         const sf::Vector2f& playerPos, bool& proceedToNextLevel) {
    (void)enemyDescending;
    (void)enemy;
    (void)deltaTime;
    if (!sentinelHasAnswered) {
        text.setString("Sentinel: Welcome to level 2. Solve this puzzle.");
        text.setPosition(200, 200); // Adjust position as needed
        playerOptions.setString("Q: Answer A \nT: Answer B");
        playerOptions.setPosition(playerPos.x, playerPos.y - 50);
        awaitingResponse = true;
    } else if (!responseComplete) {
        text.setString("Was the sentinel telling the truth? (Y/N)");

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
            checkAnswerLevel2(true, text, enemyTriggered, proceedToNextLevel);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
            checkAnswerLevel2(false, text, enemyTriggered, proceedToNextLevel);
        }
    }

    if (awaitingResponse && !sentinelHasAnswered) {
        handleQuestionResponseLevel2(text);
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
        if (sentinelTruth) {
            text.setString("No, I am an honest sentinel.");
        } else {
            text.setString("It will.");
        }
        messageDisplayTimer.restart();
        sentinelHasAnswered = true;
        awaitingResponse = false;
    }
}

void SentinelInteraction::handleQuestionResponseLevel2(sf::Text& text) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        sentinelTruth = dist(rng) == 1;
        if (sentinelTruth) {
            text.setString("That is correct.");
        } else {
            text.setString("That is incorrect.");
        }
        messageDisplayTimer.restart();
        sentinelHasAnswered = true;
        awaitingResponse = false;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !sentinelHasAnswered) {
        sentinelTruth = dist(rng) == 1;
        if (sentinelTruth) {
            text.setString("Indeed.");
        } else {
            text.setString("Not quite.");
        }
        messageDisplayTimer.restart();
        sentinelHasAnswered = true;
        awaitingResponse = false;
    }
}

void SentinelInteraction::checkAnswer(bool playerAnswer, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    if (playerAnswer == sentinelTruth) {
        text.setString("Correct! You may proceed.");
        ascent = true;
        isCorrect = true;
        proceedToNextLevel = true;
        messageDisplayTimer.restart();
    } else {
        text.setString("Incorrect. Press F near the button to try again.");
        responseComplete = true;
        sentinelHasAnswered = false;
        messageDisplayTimer.restart();
        buttonInteraction.resetPrompt();
        enemyTriggered = false;
    }
}

void SentinelInteraction::checkAnswerLevel2(bool playerAnswer, sf::Text& text, bool& enemyTriggered, bool& proceedToNextLevel) {
    if (playerAnswer == sentinelTruth) {
        text.setString("Correct! You may proceed.");
        ascent = true;
        isCorrect = true;
        proceedToNextLevel = true;
        messageDisplayTimer.restart();
    } else {
        text.setString("Incorrect. Try again.");
        responseComplete = true;
        sentinelHasAnswered = false;
        messageDisplayTimer.restart();
        enemyTriggered = false;
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
        }
    }
}

void SentinelInteraction::handleAscentAndCleanupLevel2(std::unique_ptr<Enemy>& enemy, sf::Text& text, bool& enemyTriggered, float deltaTime) {
    if (ascent) {
        float ascentSpeed = 500.0f;
        enemy->setPosition(enemy->getPosition().x, enemy->getPosition().y - ascentSpeed * deltaTime);

        if (enemy->getPosition().y + enemy->getGlobalBounds().height < 0) {
            enemy->setPosition(100, -500);
            resetState();
            enemyTriggered = false;
            text.setString("");
            ascent = false;
        }
    }
}

bool SentinelInteraction::isAscending() const {
    return ascent;
}

