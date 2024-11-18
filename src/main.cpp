#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <map>
#include <fstream>
#include <chrono>
#include <random>
#include "../include/TitleScreen.hpp"
#include "../include/nfd.h"
#include "../include/Player.hpp"
#include "../include/Enemy.hpp"
#include "../include/Background.hpp"
#include "../include/Platform.hpp"
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/cursorfont.h>
#include "AssetType.hpp"
#include "../include/ButtonInteraction.hpp"
#include "../include/SentinelInteraction.hpp"

void enableMouse();
void disableMouse();

enum class GameMode { Play, Edit };
enum class GameState { Title, Play, Exit };

bool isValidAssetType(int assetTypeInt) {
    return assetTypeInt >= static_cast<int>(AssetType::Brick) &&
           assetTypeInt <= static_cast<int>(AssetType::Statue3);
}

void drawGrid(sf::RenderWindow& window, const sf::Vector2f& viewSize, float gridSize) {
    sf::VertexArray lines(sf::Lines);
    for (float y = 0; y < viewSize.y; y += gridSize) {
        lines.append(sf::Vertex(sf::Vector2f(0, y), sf::Color(255, 255, 255, 100)));
        lines.append(sf::Vertex(sf::Vector2f(viewSize.x, y), sf::Color(255, 255, 255, 100)));
    }
    for (float x = 0; x < viewSize.x; x += gridSize) {
        lines.append(sf::Vertex(sf::Vector2f(x, 0), sf::Color(255, 255, 255, 100)));
        lines.append(sf::Vertex(sf::Vector2f(x, viewSize.y), sf::Color(255, 255, 255, 100)));
    }
    window.draw(lines);
}

void saveLevel(sf::RenderWindow& window, const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions) {
    enableMouse();
    window.create(sf::VideoMode(1280, 720), "veX - Saving...", sf::Style::Close);
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_SaveDialog("txt", nullptr, &outPath);
    if (result == NFD_OKAY) {
        std::ofstream outFile(outPath);
        for (const auto& tileData : tilePositions) {
            outFile << tileData.first.x << " " << tileData.first.y << " " << static_cast<int>(tileData.second) << "\n";
        }
        outFile.close();
    }
    window.create(sf::VideoMode(1920, 1080), "veX", sf::Style::Fullscreen);
    disableMouse();
}

std::vector<std::pair<sf::Vector2f, AssetType>> loadLevelFromFile(const std::string& filepath, std::vector<Platform>& platforms,
                                                                  const std::map<AssetType, sf::Texture>& textures) {
    std::vector<std::pair<sf::Vector2f, AssetType>> tiles;
    std::ifstream inFile(filepath);
    float x, y;
    int assetTypeInt;
    platforms.clear();
    while (inFile >> x >> y >> assetTypeInt) {
        if (!isValidAssetType(assetTypeInt)) continue;
        sf::Vector2f pos(x, y);
        AssetType assetType = static_cast<AssetType>(assetTypeInt);
        tiles.emplace_back(pos, assetType);
        auto textureIt = textures.find(assetType);
        if (textureIt != textures.end()) {
            const sf::Texture& tileTexture = textureIt->second;
            sf::Vector2f size(tileTexture.getSize().x, tileTexture.getSize().y);
            bool isGrassy = (assetType == AssetType::Grassy || assetType == AssetType::Ground || assetType == AssetType::Ground3);
            if (assetType != AssetType::Tree && assetType != AssetType::Button && assetType != AssetType::Statue3) {
                platforms.emplace_back(pos.x, pos.y, size.x, size.y, tileTexture, isGrassy);
            }
        }
    }
    inFile.close();
    return tiles;
}

std::vector<std::pair<sf::Vector2f, AssetType>> loadLevel(sf::RenderWindow& window, std::vector<Platform>& platforms,
                                                          const std::map<AssetType, sf::Texture>& textures, bool isDefault = false) {
    enableMouse();
    window.create(sf::VideoMode(1280, 720), "veX - Loading...", sf::Style::Close);
    std::vector<std::pair<sf::Vector2f, AssetType>> tiles;
    if (isDefault) {
        tiles = loadLevelFromFile("levels/level1.txt", platforms, textures);
    } else {
        nfdchar_t* outPath = nullptr;
        nfdresult_t result = NFD_OpenDialog("txt", nullptr, &outPath);
        if (result == NFD_OKAY) {
            tiles = loadLevelFromFile(outPath, platforms, textures);
        }
    }
    window.create(sf::VideoMode(1920, 1080), "veX", sf::Style::Fullscreen);
    disableMouse();
    return tiles;
}

void updateView(sf::RenderWindow& window, sf::View& view) {
    float baseWidth = 1920.0f;
    float baseHeight = 1080.0f;
    view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    view.setSize(baseWidth, baseHeight);
    window.setView(view);
}

void disableMouse() {
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);
    Cursor invisibleCursor;
    Pixmap bitmapNoData;
    XColor black;
    static char noData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    black.red = black.green = black.blue = 0;
    bitmapNoData = XCreateBitmapFromData(display, root, noData, 8, 8);
    invisibleCursor = XCreatePixmapCursor(display, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
    XDefineCursor(display, root, invisibleCursor);
    XFreeCursor(display, invisibleCursor);
    XFreePixmap(display, bitmapNoData);
    XFlush(display);
    XCloseDisplay(display);
}

void enableMouse() {
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);
    XUndefineCursor(display, root);
    XFlush(display);
    XCloseDisplay(display);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "veX", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);

    std::unique_ptr<Player> player = std::make_unique<Player>(0, 950);
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(1600, -500);
    TitleScreen titleScreen(window);

    GameState gameState = GameState::Title;
    GameMode currentMode = GameMode::Play;
    sf::Clock clock;
    bool debugMode = false;
    const float gridSize = 64.0f;
    std::vector<Platform> platforms;
    
    bool playerJustReset = false;

    AssetType currentAsset = AssetType::Brick;

    std::map<AssetType, sf::Texture> textureMap;
    if (!textureMap[AssetType::Brick].loadFromFile("assets/tutorial_level/left_grass.png") ||
        !textureMap[AssetType::Dripstone].loadFromFile("assets/tutorial_level/dripstone.png") ||
        !textureMap[AssetType::LeftRock].loadFromFile("assets/tutorial_level/left_rock.png") ||
        !textureMap[AssetType::RightRock].loadFromFile("assets/tutorial_level/right_rock.png") ||
        !textureMap[AssetType::Tree].loadFromFile("assets/tutorial_level/tree.png") ||
        !textureMap[AssetType::Grassy].loadFromFile("assets/tutorial_level/grassy.png") ||
        !textureMap[AssetType::Button].loadFromFile("assets/tutorial_level/button.png") ||
        !textureMap[AssetType::Stair1].loadFromFile("assets/level2/stair1.png") ||
        !textureMap[AssetType::Stair2].loadFromFile("assets/level2/stair2.png") ||
        !textureMap[AssetType::RightStair1].loadFromFile("assets/level2/rightstair1.png") ||
        !textureMap[AssetType::RightStair2].loadFromFile("assets/level2/rightstair2.png") ||
        !textureMap[AssetType::Ground].loadFromFile("assets/level2/floor.png") ||
        !textureMap[AssetType::Ground3].loadFromFile("assets/level3/ground.png") ||
        !textureMap[AssetType::Platform3].loadFromFile("assets/level3/platform.png") ||
        !textureMap[AssetType::Brick3].loadFromFile("assets/level3/brick.png") ||
        !textureMap[AssetType::Statue3].loadFromFile("assets/level3/statue.png")) {
        std::cerr << "Failed to load textures" << std::endl;
        return -1;
    }

    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", sf::Vector2u(1920, 1080));
    Background nextLevelBackground("assets/level2/background.png",
                                   "assets/level2/middleground.png",
                                   "assets/level2/foreground.png", sf::Vector2u(1920, 1080));
    Background level3Background("assets/level3/clouds.png",
                                "assets/level3/town.png",
                                "assets/tutorial_level/middleground.png", sf::Vector2u(1920, 1080));

    std::vector<std::pair<sf::Vector2f, AssetType>> tilePositions = loadLevel(window, platforms, textureMap, true);

    ButtonInteraction buttonInteraction;
    SentinelInteraction sentinelInteraction;

    bool enemyTriggered = false;
    bool enemyDescending = false;
    bool enemySpawned = false;
    bool proceedToNextLevel = false;
    int currentLevel = 1;
    bool sentinelDescendLevel2 = false;
    bool sentinelDescendLevel3 = false;  // Add this line

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (gameState == GameState::Title) {
                enableMouse();
                titleScreen.handleInput();
                if (titleScreen.currentSelection == 0 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
                    gameState = GameState::Play;
                    disableMouse();
                } else if (titleScreen.currentSelection == 2 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
                    gameState = GameState::Exit;
                    window.close();
                }
            }

            if (gameState == GameState::Play) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                    currentMode = (currentMode == GameMode::Play) ? GameMode::Edit : GameMode::Play;
                    currentMode == GameMode::Edit ? enableMouse() : disableMouse();
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D && currentMode == GameMode::Edit) {
                    debugMode = !debugMode;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                    saveLevel(window, tilePositions);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
                    tilePositions = loadLevel(window, platforms, textureMap, false);
                }

                if (currentLevel == 1) {
                    if (event.key.code == sf::Keyboard::Num1) currentAsset = AssetType::Brick;
                    if (event.key.code == sf::Keyboard::Num2) currentAsset = AssetType::Dripstone;
                    if (event.key.code == sf::Keyboard::Num3) currentAsset = AssetType::LeftRock;
                    if (event.key.code == sf::Keyboard::Num4) currentAsset = AssetType::RightRock;
                    if (event.key.code == sf::Keyboard::Num6) currentAsset = AssetType::Grassy;
                } else if (currentLevel == 2) {
                    if (event.key.code == sf::Keyboard::Num1) currentAsset = AssetType::Stair1;
                    if (event.key.code == sf::Keyboard::Num2) currentAsset = AssetType::Stair2;
                    if (event.key.code == sf::Keyboard::Num3) currentAsset = AssetType::RightStair1;
                    if (event.key.code == sf::Keyboard::Num4) currentAsset = AssetType::RightStair2;
                    if (event.key.code == sf::Keyboard::Num5) currentAsset = AssetType::Button;
                    if (event.key.code == sf::Keyboard::Num6) currentAsset = AssetType::Ground;
                } else if (currentLevel == 3) {
                    if (event.key.code == sf::Keyboard::Num1) currentAsset = AssetType::Ground3;
                    if (event.key.code == sf::Keyboard::Num2) currentAsset = AssetType::Platform3;
                    if (event.key.code == sf::Keyboard::Num3) currentAsset = AssetType::Brick3;
                    if (event.key.code == sf::Keyboard::Num4) currentAsset = AssetType::Statue3;
                    if (event.key.code == sf::Keyboard::Num5) currentAsset = AssetType::Button;
                }
            }

            if (currentMode == GameMode::Edit) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    sf::Vector2f tilePos(static_cast<float>(static_cast<int>(worldPos.x / gridSize) * gridSize),
                         static_cast<float>(static_cast<int>(worldPos.y / gridSize) * gridSize));

    // Existing code for adding tiles with left-click
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (std::find_if(tilePositions.begin(), tilePositions.end(),
                         [&](const std::pair<sf::Vector2f, AssetType>& tile) { return tile.first == tilePos; }) == tilePositions.end()) {
            tilePositions.emplace_back(tilePos, currentAsset);
            sf::Texture tileTexture = textureMap[currentAsset];
            sf::Vector2f size(tileTexture.getSize().x, tileTexture.getSize().y);

            if (currentAsset == AssetType::Grassy || currentAsset == AssetType::Ground || currentAsset == AssetType::Ground3) {
                platforms.emplace_back(tilePos.x, tilePos.y, size.x, size.y, tileTexture, true);
            } else if (currentAsset != AssetType::Tree && currentAsset != AssetType::Button && currentAsset != AssetType::Statue3) {
                platforms.emplace_back(tilePos.x, tilePos.y, size.x, size.y, tileTexture, false);
            }
        }
    }

    // Add this code to handle removing tiles with right-click
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        // Find the tile at tilePos in tilePositions
        auto tileIt = std::find_if(tilePositions.begin(), tilePositions.end(),
                                   [&](const std::pair<sf::Vector2f, AssetType>& tile) {
                                       return tile.first == tilePos;
                                   });
        if (tileIt != tilePositions.end()) {
            // Remove the tile from tilePositions
            tilePositions.erase(tileIt);

            // Also remove the corresponding Platform from platforms
            auto platformIt = std::find_if(platforms.begin(), platforms.end(),
                                           [&](const Platform& platform) {
                                               const auto& tiles = platform.getTiles();
                                               for (const auto& tile : tiles) {
                                                   if (tile.getPosition() == tilePos) {
                                                       return true;
                                                   }
                                               }
                                               return false;
                                           });
            if (platformIt != platforms.end()) {
                platforms.erase(platformIt);
            }
        }
    }
}

        }

        float deltaTime = clock.restart().asSeconds();

        if (gameState == GameState::Title) {
            titleScreen.handleInput();
            titleScreen.update(deltaTime);
            titleScreen.render();
        } else if (gameState == GameState::Play) {
            window.clear();
            float playerX = player->getGlobalBounds().left;

            if (currentLevel == 1) {
                background.render(window, sf::Vector2u(1920, 1080), playerX, deltaTime);
            } else if (currentLevel == 2) {
                nextLevelBackground.render(window, sf::Vector2u(1920, 1080), playerX, deltaTime);
                if (sentinelDescendLevel2) {
                    float targetYPosition = 200.0f;
                    float descentSpeed = 500.0f;
                    if (enemy->getPosition().y < targetYPosition) {
                        enemy->setPosition(100, enemy->getPosition().y + descentSpeed * deltaTime);
                    } else {
                        enemy->setPosition(100, targetYPosition);
                        sentinelDescendLevel2 = false;
                        enemyTriggered = true;
                        sentinelInteraction.startLevel2Interaction();
                    }
                }
            // Replace the Level 3 section in main.cpp with this corrected version
            } else if (currentLevel == 3) {
                  level3Background.render(window, sf::Vector2u(1920, 1080), playerX, deltaTime);
    
                  if (sentinelDescendLevel3) {
                      float targetYPosition = 200.0f;
                      float descentSpeed = 500.0f;
                      if (enemy->getPosition().y < targetYPosition) {
                          enemy->setPosition(1600, enemy->getPosition().y + descentSpeed * deltaTime);
                      } else {
                          enemy->setPosition(1600, targetYPosition);
                          sentinelDescendLevel3 = false;
                          enemyTriggered = true;
                          sentinelInteraction.startLevel3Interaction();
                        }
                  }

                  if (enemyTriggered) {
                      sentinelInteraction.triggerInteractionLevel3(window, text, enemyTriggered, 
                                                                  enemyDescending, enemySpawned, 
                                                                  enemy, deltaTime, player->getPosition(), 
                                                                  buttonInteraction, proceedToNextLevel);
                      window.draw(text);
                  }

                  // Handle button interaction for level 3
                  buttonInteraction.handleInteractionLevel3(player->getPosition(), tilePositions, 
                                                            window, enemyTriggered, enemyDescending, 
                                                            sentinelDescendLevel3);
            }             
          

            for (const auto& tileData : tilePositions) {
                auto it = textureMap.find(tileData.second);
                if (it != textureMap.end()) {
                    sf::Sprite tile(it->second);
                    tile.setPosition(tileData.first);
                    window.draw(tile);
                } else {
                    std::cerr << "Warning: Texture not found for AssetType " << static_cast<int>(tileData.second) << std::endl;
                }
            }

            if (currentLevel == 1) {
                sentinelInteraction.triggerInteraction(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, player->getPosition(), buttonInteraction, proceedToNextLevel);
                window.draw(text);
            } else if (currentLevel == 2) {
                if (enemyTriggered) {
                    sentinelInteraction.triggerInteractionLevel2(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, player->getPosition(), buttonInteraction, proceedToNextLevel);
                    window.draw(text);
                }
            } else if (currentLevel == 3) {
            }

            if (currentMode == GameMode::Play) {
                if (playerJustReset) playerJustReset = false;
                else player->update(deltaTime, platforms, window.getSize().x, window.getSize().y, *enemy);

                if (!sentinelInteraction.isAscending()) {
                    enemy->update(deltaTime, platforms, window.getSize().x, window.getSize().y);
                }

                player->draw(window);
                enemy->draw(window);

                if (currentLevel == 1) {
                    buttonInteraction.handleInteraction(player->getPosition(), tilePositions, window, enemyTriggered, enemyDescending, enemySpawned);
                }
                
                if (currentLevel == 2) {
                    if (enemyTriggered) {
                        sentinelInteraction.triggerInteractionLevel2(window, text, enemyTriggered, 
                                                                    enemyDescending, enemySpawned, 
                                                                    enemy, deltaTime, player->getPosition(), buttonInteraction, 
                                                                    proceedToNextLevel);
                        window.draw(text);
                    }
                    buttonInteraction.handleInteractionLevel2(player->getPosition(), tilePositions, 
                                                              window, enemyTriggered, enemyDescending, 
                                                              sentinelDescendLevel2);
                }
                if (currentLevel == 3) {
                    buttonInteraction.handleInteractionLevel3(player->getPosition(), tilePositions, 
                                                              window, enemyTriggered, enemyDescending, 
                                                              sentinelDescendLevel3);  // Use the new Level 3 handler
                }
            }

            if (currentMode == GameMode::Edit && debugMode) {
                drawGrid(window, view.getSize(), gridSize);
            }

            if (proceedToNextLevel && player->getPosition().x >= window.getSize().x - 75) {
                proceedToNextLevel = false;
                if (currentLevel == 1) {
                    currentLevel = 2;
                    tilePositions.clear();
                    platforms.clear();
                    
                    tilePositions = loadLevelFromFile("levels/level2.txt", platforms, textureMap);

                    enemy->setPosition(100, -500);
                    player->setPosition(0, 850);
                    player->resetState();
                    enemy->flipSprite();
                    updateView(window, view);
                    enemyTriggered = false;
                    enemySpawned = false;
                    sentinelInteraction.resetState();
                    text.setString("");
                    buttonInteraction.resetPrompt();
                    playerJustReset = true;
                    sentinelDescendLevel2 = false;
                } else if (currentLevel == 2) {
                    currentLevel = 3;
                    tilePositions.clear();
                    platforms.clear();

                    tilePositions = loadLevelFromFile("levels/level3.txt", platforms, textureMap);

                    enemy->setPosition(1600, -500);
                    enemy->flipSprite();
                    player->setPosition(0, 850);
                    player->resetState();
                    updateView(window, view);
                    enemyTriggered = false;
                    enemySpawned = false;
                    sentinelInteraction.resetState();
                    text.setString("");
                    buttonInteraction.resetPrompt();
                    playerJustReset = true;
                    sentinelDescendLevel3 = false;
                }
            }

            window.display();
        }
    }

    return 0;
}

