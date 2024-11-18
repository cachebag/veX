// ButtonInteraction.hpp

#ifndef BUTTON_INTERACTION_HPP
#define BUTTON_INTERACTION_HPP

#include <SFML/Graphics.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <utility>
#include "AssetType.hpp"

class ButtonInteraction {
public:
    ButtonInteraction();
    void handleInteraction(const sf::Vector2f& playerPos, const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions,
                           sf::RenderWindow& window, bool& enemyTriggered, bool& enemyDescending, bool& enemySpawned);

    void handleInteractionLevel2(const sf::Vector2f& playerPos, const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions, 
                           sf::RenderWindow& window, bool& enemyTriggered, bool& enemyDescending, bool& sentinelDescendLevel2);

    void handleInteractionLevel3(const sf::Vector2f& playerPos, const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions, 
                           sf::RenderWindow& window, bool& enemyTriggered, bool& enemyDescending, bool& sentinelDescendLevel3);
    void resetPrompt();
    void resetAllFlags();  // Add this new method

private:
    sf::Font font;
    sf::Text text;
    bool showingText;
    bool promptVisible;
    int displayDuration;
    std::chrono::steady_clock::time_point timerStart;
    bool interactionInProgress;

    float distance(const sf::Vector2f& a, const sf::Vector2f& b);
};

extern bool resetSentinelInteraction;

#endif // BUTTON_INTERACTION_HPP

