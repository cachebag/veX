// ButtonInteraction.cpp

#include "../include/ButtonInteraction.hpp"

bool resetSentinelInteraction = false;

ButtonInteraction::ButtonInteraction()
    : showingText(false), promptVisible(true), displayDuration(3),
      timerStart(std::chrono::steady_clock::now()), interactionInProgress(false) {
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Failed to load font\n";
    }
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
}

void ButtonInteraction::handleInteraction(const sf::Vector2f& playerPos,
                                          const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions,
                                          sf::RenderWindow& window, bool& enemyTriggered, bool& enemyDescending,
                                          bool& enemySpawned) {
    bool nearButton = false;

    for (const auto& tile : tilePositions) {
        if (tile.second == AssetType::Button && distance(playerPos, tile.first) < 100.0f) {
            nearButton = true;
            if (promptVisible && !interactionInProgress) {
                if (!enemySpawned) {
                    text.setString("Press F to prompt the sentinel...");
                } else {
                    text.setString("Press F to try again...");
                }
                text.setPosition(playerPos.x, playerPos.y - 50);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && !interactionInProgress) {
                timerStart = std::chrono::steady_clock::now();
                showingText = true;
                promptVisible = false;
                enemyTriggered = true;
                enemyDescending = !enemySpawned;
                enemySpawned = true;
                interactionInProgress = true;
                text.setString("");

                resetSentinelInteraction = true;
            }
        }
    }

    if (showingText) {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timerStart).count();
        if (elapsed >= displayDuration) {
            showingText = false;
        }
    }

    if ((nearButton && promptVisible) || showingText) {
        window.draw(text);
    }
}

void ButtonInteraction::handleInteractionLevel2(const sf::Vector2f& playerPos, 
                                              const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions,
                                              sf::RenderWindow& window, bool& enemyTriggered, 
                                              bool& enemyDescending, bool& sentinelDescendLevel2) {
    bool nearButton = false;

    for (const auto& tile : tilePositions) {
        if (tile.second == AssetType::Button) {
            sf::Vector2f buttonPos = tile.first;
            if (distance(playerPos, buttonPos) < 100.0f) {
                nearButton = true;
                if (promptVisible && !interactionInProgress) {
                    text.setString("Press F to prompt the sentinel...");
                    text.setPosition(buttonPos.x - 50, buttonPos.y - 50);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && !interactionInProgress) {
                    timerStart = std::chrono::steady_clock::now();
                    showingText = true;
                    promptVisible = false;
                    enemyTriggered = true;
                    enemyDescending = true;
                    sentinelDescendLevel2 = true;
                    interactionInProgress = true;
                    text.setString("");

                    resetSentinelInteraction = true;
                }
            }
        }
    }

    if (showingText) {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timerStart).count();
        if (elapsed >= displayDuration) {
            showingText = false;
        }
    }

    if ((nearButton && promptVisible) || showingText) {
        window.draw(text);
    }
}

void ButtonInteraction::handleInteractionLevel3(const sf::Vector2f& playerPos, 
                                              const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions,
                                              sf::RenderWindow& window, bool& enemyTriggered, 
                                              bool& enemyDescending, bool& sentinelDescendLevel3) {
    bool nearButton = false;

    for (const auto& tile : tilePositions) {
        if (tile.second == AssetType::Button) {
            sf::Vector2f buttonPos = tile.first;
            // Use the same distance check as other levels
            if (distance(playerPos, buttonPos) < 100.0f) {
                nearButton = true;
                if (promptVisible && !interactionInProgress) {
                    text.setString("Press F...");  // Make consistent with other levels
                    text.setPosition(buttonPos.x - 50, buttonPos.y - 50);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && !interactionInProgress) {
                    // Debug output to verify the button press
                    std::cout << "Level 3 button pressed\n";
                    
                    timerStart = std::chrono::steady_clock::now();
                    showingText = true;
                    promptVisible = false;
                    
                    // Set these flags before sentinelDescendLevel3
                    enemyTriggered = true;
                    enemyDescending = true;
                    sentinelDescendLevel3 = true;
                    
                    interactionInProgress = true;
                    text.setString("");
                    
                    resetSentinelInteraction = true;

                    // Debug output to verify flags
                    std::cout << "enemyTriggered: " << enemyTriggered << "\n";
                    std::cout << "sentinelDescendLevel3: " << sentinelDescendLevel3 << "\n";
                }
            }
        }
    }

    if (showingText) {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timerStart).count();
        if (elapsed >= displayDuration) {
            showingText = false;
        }
    }

    if ((nearButton && promptVisible) || showingText) {
        window.draw(text);
    }
}

void ButtonInteraction::resetAllFlags() {
    promptVisible = true;
    interactionInProgress = false;
    showingText = false;
}

void ButtonInteraction::resetPrompt() {
    promptVisible = true;
    interactionInProgress = false;
}

float ButtonInteraction::distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

