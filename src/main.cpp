// main.cpp

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

#ifdef __APPLE__
void enableMouse(sf::RenderWindow& window) {
    window.setMouseCursorVisible(true);
}

void disableMouse(sf::RenderWindow& window) {
    window.setMouseCursorVisible(false);
}
#else
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/cursorfont.h>
#include "AssetType.hpp"
#include "../include/ButtonInteraction.hpp"
#include "../include/SentinelInteraction.hpp"

// Function declarations
void enableMouse();
void disableMouse();
=======
void enableMouse() {
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);
    XUndefineCursor(display, root);
    XFlush(display);
    XCloseDisplay(display);
}

void disableMouse() {
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);
    Cursor invisibleCursor;
    Pixmap bitmapNoData;
    XColor black;
    static char noData[] = { 0,0,0,0,0,0,0,0 };
    black.red = black.green = black.blue = 0;
    bitmapNoData = XCreateBitmapFromData(display, root, noData, 8, 8);
    invisibleCursor = XCreatePixmapCursor(display, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
    XDefineCursor(display, root, invisibleCursor);
    XFreeCursor(display, invisibleCursor);
    XFreePixmap(bitmapNoData);
    XFlush(display);
    XCloseDisplay(display);
}
#endif

// Enums and utility functions
enum class GameMode { Play, Edit };
enum class GameState { Title, Play, Exit };

bool isValidAssetType(int assetTypeInt) {
    return assetTypeInt >= static_cast<int>(AssetType::Brick) &&
           assetTypeInt <= static_cast<int>(AssetType::Ground);
}

// Draw grid function
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

// Save level function
void saveLevel(sf::RenderWindow& window, const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions) {
    #ifdef __APPLE__
    enableMouse(window);
    #else
    enableMouse();
    #endif
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
    #ifdef __APPLE__
    disableMouse(window);
    #else
    disableMouse();
    #endif
}

// Load level from file function
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
            bool isGrassy = (assetType == AssetType::Grassy || assetType == AssetType::Ground);
            platforms.emplace_back(pos.x, pos.y, size.x, size.y, tileTexture, isGrassy);
        }
    }
    inFile.close();
    return tiles;
}

// Load level function
std::vector<std::pair<sf::Vector2f, AssetType>> loadLevel(sf::RenderWindow& window, std::vector<Platform>& platforms,
                                                          const std::map<AssetType, sf::Texture>& textures, bool isDefault = false) {
    #ifdef __APPLE__
    enableMouse(window);
    #else
    enableMouse();
    #endif
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
    window.create(sf::VideoMode(1920, 1080), "veX", sf::Style::Default);
    #ifdef __APPLE__
    disableMouse(window);
    #else
    disableMouse();
    #endif
    return tiles;
}

// Update view function
void updateView(sf::RenderWindow& window, sf::View& view) {
    float baseWidth = 1920.0f;
    float baseHeight = 1080.0f;
    view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    view.setSize(baseWidth, baseHeight);
    window.setView(view);
}

// Mouse functions
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

class ButtonInteraction {
public:
    ButtonInteraction() : showingText(false), promptVisible(true), displayDuration(5), timerStart(std::chrono::steady_clock::now()) {
        if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
            std::cerr << "Failed to load font\n";
        }
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
    }

    void handleInteraction(const sf::Vector2f& playerPos, const std::vector<std::pair<sf::Vector2f, AssetType>>& tilePositions, sf::RenderWindow& window, bool& enemyTriggered, bool& enemyDescending, bool& enemySpawned) {
        bool nearButton = false;

        for (const auto& tile : tilePositions) {
            if (tile.second == AssetType::Button && distance(playerPos, tile.first) < 100.0f) {
                nearButton = true;
                if (promptVisible) {
                    text.setString("Press F to prompt the sentinel...");
                    text.setPosition(playerPos.x, playerPos.y - 50);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && !enemySpawned) {
                    timerStart = std::chrono::steady_clock::now();
                    showingText = true;
                    promptVisible = false;
                    enemyTriggered = true;
                    enemyDescending = true;
                    enemySpawned = true;
                    text.setString("");
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

    void resetPrompt() {
        promptVisible = true;
    }

private:
    sf::Font font;
    sf::Text text;
    bool showingText;
    bool promptVisible;
    int displayDuration;
    std::chrono::steady_clock::time_point timerStart;

    float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
        return std::sqrt((a.x - b.x) * (a.x - b.y));
    }
};

class SentinelInteraction {
public:
    SentinelInteraction() 
        : questionVisible(false), ascent(false), awaitingResponse(false), responseComplete(false), 
          enemyGone(false), rng(rd()), dist(0, 1) {
        
        if (!font.loadFromFile("assets/fonts/Merriweather-Regular.ttf")) {
            std::cerr << "Failed to load font\n";
        }

        playerOptions.setFont(font);
        playerOptions.setCharacterSize(24);
        playerOptions.setFillColor(sf::Color::White);
    }

    void triggerInteraction(sf::RenderWindow& window, sf::Text& text, bool& enemyTriggered, bool& enemyDescending, bool& enemySpawned, 
                            std::unique_ptr<Enemy>& enemy, float deltaTime, const sf::Vector2f& playerPos, ButtonInteraction buttonInteraction) {
        if (enemyGone) return;

        if (!enemyTriggered) return;

        sf::FloatRect enemyBounds = enemy->getGlobalBounds();
        float targetYPosition = 600.0f;

        if (enemyBounds.top < targetYPosition && !ascent) {
            enemy->setPosition(enemyBounds.left, enemyBounds.top + 500.0f * deltaTime);
        } else if (!ascent && !responseComplete) {
            enemyDescending = false;
            questionVisible = true;
            text.setString("What do you seek by summoning me?");
            text.setPosition(1400, 500);

            playerOptions.setString("Q: Nothing, go away \n T: Will your next response be a lie?");
            playerOptions.setPosition(playerPos.x, playerPos.y - 50);

            awaitingResponse = true;
        }

        if (questionVisible && awaitingResponse) {
            handleInput(text, enemyTriggered, enemySpawned, buttonInteraction);
        }

        if (ascent) {
            if (enemyBounds.top > -500.0f) {
                ascent = true;
                enemy->setPosition(enemyBounds.left, enemyBounds.top - 500.0f * deltaTime);
            } else {
                ascent = false;
                enemySpawned = false;
                questionVisible = false;
                responseComplete = false;
                enemyGone = true;
                buttonInteraction.resetPrompt();
            }
        }

        if (awaitingResponse) {
            window.draw(playerOptions);
        }
    }

private:
    sf::Font font;
    sf::Text playerOptions;
    bool questionVisible;
    bool ascent;
    bool awaitingResponse;
    bool responseComplete;
    bool enemyGone;
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

    void handleInput(sf::Text& text, bool& enemyTriggered, bool& enemySpawned, ButtonInteraction& buttonInteraction) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            questionVisible = false;
            ascent = true;
            awaitingResponse = false;
            responseComplete = true;
            text.setString("");

            enemyTriggered = false;
            enemySpawned = true;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !responseComplete) {
            int response = dist(rng);
            if (response == 0) {
                text.setString("I am an honest sentinel.");
            } else {
                text.setString("I am a dishonest sentinel.");
            }
            questionVisible = false;
            awaitingResponse = false;
            responseComplete = true;
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "veX", sf::Style::Default);
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
    std::map<AssetType, sf::Texture> level1Textures;
    std::map<AssetType, sf::Texture> level2Textures;
    bool playerJustReset = false;

    AssetType currentAsset = AssetType::Brick;

    // Load textures for level 1
    if (!level1Textures[AssetType::Brick].loadFromFile("assets/tutorial_level/left_grass.png") ||
        !level1Textures[AssetType::Dripstone].loadFromFile("assets/tutorial_level/dripstone.png") ||
        !level1Textures[AssetType::LeftRock].loadFromFile("assets/tutorial_level/left_rock.png") ||
        !level1Textures[AssetType::RightRock].loadFromFile("assets/tutorial_level/right_rock.png") ||
        !level1Textures[AssetType::Tree].loadFromFile("assets/tutorial_level/tree.png") ||
        !level1Textures[AssetType::Grassy].loadFromFile("assets/tutorial_level/grassy.png") ||
        !level1Textures[AssetType::Button].loadFromFile("assets/tutorial_level/button.png")) {
        return -1;
    }

    // Load textures for level 2
    if (!level2Textures[AssetType::Stair1].loadFromFile("assets/level2/stair1.png") ||
        !level2Textures[AssetType::Stair2].loadFromFile("assets/level2/stair2.png") ||
        !level2Textures[AssetType::RightStair1].loadFromFile("assets/level2/rightstair1.png") ||
        !level2Textures[AssetType::RightStair2].loadFromFile("assets/level2/rightstair2.png") ||
        !level2Textures[AssetType::Ground].loadFromFile("assets/level2/floor.png")) {
        return -1;
    }

    std::map<AssetType, sf::Texture>* activeTextures = &level1Textures;

    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", sf::Vector2u(1920, 1080));
    Background nextLevelBackground("assets/level2/background.png",
                                   "assets/level2/middleground.png",
                                   "assets/level2/foreground.png", sf::Vector2u(1920, 1080));

    std::vector<std::pair<sf::Vector2f, AssetType>> tilePositions = loadLevel(window, platforms, *activeTextures, true);

    ButtonInteraction buttonInteraction;
    SentinelInteraction sentinelInteraction;

    bool enemyTriggered = false;
    bool enemyDescending = false;
    bool enemySpawned = false;
    bool proceedToNextLevel = false;
    int currentLevel = 1;
    bool sentinelDescendLevel2 = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (gameState == GameState::Title) {
                #ifdef __APPLE__
                enableMouse(window);
                #else
                enableMouse();
                #endif
                titleScreen.handleInput();
                if (titleScreen.currentSelection == 0 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
                    gameState = GameState::Play;
                    #ifdef __APPLE__
                    disableMouse(window);
                    #else
                    disableMouse();
                    #endif
                } else if (titleScreen.currentSelection == 2 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
                    gameState = GameState::Exit;
                    window.close();
                }
            }

            if (gameState == GameState::Play) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                    currentMode = (currentMode == GameMode::Play) ? GameMode::Edit : GameMode::Play;
                    currentMode == GameMode::Edit ? enableMouse() : disableMouse();
                    if (currentMode == GameMode::Edit) {
                        #ifdef __APPLE__
                        enableMouse(window);
                        #else
                        enableMouse();
                        #endif
                    } else {
                        #ifdef __APPLE__
                        disableMouse(window);
                        #else
                        disableMouse();
                        #endif
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D && currentMode == GameMode::Edit) {
                    debugMode = !debugMode;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                    saveLevel(window, tilePositions);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
                    tilePositions = loadLevel(window, platforms, *activeTextures, false);
                }

                // Asset selection in edit mode, based on the current level
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
                    if (event.key.code == sf::Keyboard::Num6) currentAsset = AssetType::Ground;
                }
            }

            if (currentMode == GameMode::Edit) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                sf::Vector2f tilePos(static_cast<float>(static_cast<int>(worldPos.x / gridSize) * gridSize),
                                     static_cast<float>(static_cast<int>(worldPos.y / gridSize) * gridSize));

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (std::find_if(tilePositions.begin(), tilePositions.end(),
                                     [&](const std::pair<sf::Vector2f, AssetType>& tile) { return tile.first == tilePos; }) == tilePositions.end()) {
                        tilePositions.emplace_back(tilePos, currentAsset);
                        sf::Texture tileTexture = (*activeTextures)[currentAsset];
                        sf::Vector2f size(tileTexture.getSize().x, tileTexture.getSize().y);

                        if (currentAsset == AssetType::Grassy || currentAsset == AssetType::Ground) {
                            platforms.emplace_back(tilePos.x, tilePos.y, size.x, size.y, tileTexture, true);
                        } else if (currentAsset != AssetType::Tree && currentAsset != AssetType::Button) {
                            platforms.emplace_back(tilePos.x, tilePos.y, size.x, size.y, tileTexture, false);
                        }
                    }
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    auto it = std::find_if(tilePositions.begin(), tilePositions.end(),
                                           [&](const std::pair<sf::Vector2f, AssetType>& tile) { return tile.first == tilePos; });
                    if (it != tilePositions.end()) {
                        int index = std::distance(tilePositions.begin(), it);
                        tilePositions.erase(it);
                        if (currentAsset != AssetType::Tree && currentAsset != AssetType::Button) {
                            platforms.erase(platforms.begin() + index);
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
            }

            for (const auto& tileData : tilePositions) {
                auto it = activeTextures->find(tileData.second);
                if (it != activeTextures->end()) {
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
                    sentinelInteraction.triggerInteractionLevel2(window, text, enemyTriggered, enemyDescending, enemySpawned, enemy, deltaTime, player->getPosition(), proceedToNextLevel);
                    window.draw(text);
                }
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
            }

            if (currentMode == GameMode::Edit && debugMode) {
                drawGrid(window, view.getSize(), gridSize);
            }

            if (proceedToNextLevel && player->getPosition().x >= window.getSize().x - 75) {
                proceedToNextLevel = false;
                currentLevel = 2;
                tilePositions.clear();
                platforms.clear();
                
                activeTextures = &level2Textures; // Switch to level 2 textures
                tilePositions = loadLevelFromFile("levels/level2.txt", platforms, *activeTextures);

                enemy->setPosition(100, -500);
                player->setPosition(0, 850);
                player->resetState();
                updateView(window, view);
                enemyTriggered = false;
                enemySpawned = false;
                sentinelInteraction.resetState();
                buttonInteraction.resetPrompt();
                playerJustReset = true;
                sentinelDescendLevel2 = true;
            }

            window.display();
        }
    }

    return 0;
}

