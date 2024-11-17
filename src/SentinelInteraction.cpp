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

void SentinelInteraction::triggerInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending,
                                             bool& enemySpawned, std::unique_ptr<Enemy>& enemy, float deltaTime,
                                             const sf::Vector2f& playerPos, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    if (currentLevel == 1) {
        triggerInteractionLevel1(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, playerPos, buttonInteraction, proceedToNextLevel);
    } else if (currentLevel == 2) {
        triggerInteractionLevel2(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, playerPos, buttonInteraction, proceedToNextLevel);
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
        handleAscentAndCleanup(enemy, text, enemyTriggered, enemySpawned, buttonInteraction,  deltaTime);
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
        text.setPosition(200, 250);
        playerOptions.setString("Q: Nothing  \nT: Did the last Sentinel lie to me?");
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

void SentinelInteraction::handleQuestionResponse(sf::Text& text) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        questionVisible = false;
        ascent = true;
        awaitingResponse = false;
        text.setString("");
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !sentinelHasAnswered) {
        sentinelTruth = dist(rng) == 1;
        
        if (dist(rng) == 1) {
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && !sentinelHasAnswered) {
        questionVisible = false;
        ascent = true;
        awaitingResponse = false;
        text.setString("");
        // Reset button interaction state when Q is pressed
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

void SentinelInteraction::checkAnswer(bool playerAnswer, sf::Text& text, bool& enemyTriggered, ButtonInteraction& buttonInteraction, bool& proceedToNextLevel) {
    // Randomly determine if the player's answer is correct, regardless of their actual choice
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
    bool randomOutcome = dist(rng) == 1;
    
    if (randomOutcome) {
        text.setString("Correct. You may proceed.");
        ascent = true;
        isCorrect = true;
        proceedToNextLevel = true;
    } else {
        text.setString("Incorrect. Try again.");
        //enemyTriggered = false;
        buttonInteraction.resetPrompt();
        resetSentinelInteraction = true;
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
            resetSentinelInteraction = true;  // Make sure button can be pressed again
        }
    }
}
bool SentinelInteraction::isAscending() const {
    return ascent;
}

