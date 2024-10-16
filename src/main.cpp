#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include "../include/TitleScreen.hpp"
#include "../include/nfd.h"
#include "../include/Player.hpp"
#include "../include/Enemy.hpp"
#include "../include/Background.hpp"
#include "../include/Platform.hpp"
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/cursorfont.h>

void enableMouse();
void disableMouse();

enum class GameMode { Play, Edit };
enum class GameState { Title, Play, Exit };
enum class AssetType { Brick = 1, Dripstone = 2, LeftRock = 3, RightRock = 4, Tree = 5, Grassy = 6, Button = 7 };

bool isValidAssetType(int assetTypeInt) {
    return assetTypeInt >= static_cast<int>(AssetType::Brick) &&
           assetTypeInt <= static_cast<int>(AssetType::Button);
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
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
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
            if (assetType != AssetType::Tree) {
                platforms.emplace_back(pos.x, pos.y, size.x, size.y, tileTexture, assetType == AssetType::Grassy);
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
    view.setCenter(baseWidth / 2.0f, baseHeight / 2.0f);
    window.setView(view);
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
    disableMouse();
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "veX", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    sf::View view;
    updateView(window, view);

    TitleScreen titleScreen(window);

    GameState gameState = GameState::Title;
    sf::Clock clock;
    std::unique_ptr<Player> player = std::make_unique<Player>(0, 500);
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(1600, 600);

    GameMode currentMode = GameMode::Play;
    bool debugMode = false;
    const float gridSize = 64.0f;
    std::vector<Platform> platforms;
    std::map<AssetType, sf::Texture> textures;
    
    AssetType currentAsset = AssetType::Brick;
    
    if (!textures[AssetType::Brick].loadFromFile("assets/tutorial_level/left_grass.png") ||
        !textures[AssetType::Dripstone].loadFromFile("assets/tutorial_level/dripstone.png") ||
        !textures[AssetType::LeftRock].loadFromFile("assets/tutorial_level/left_rock.png") ||
        !textures[AssetType::RightRock].loadFromFile("assets/tutorial_level/right_rock.png") ||
        !textures[AssetType::Tree].loadFromFile("assets/tutorial_level/tree.png") ||
        !textures[AssetType::Grassy].loadFromFile("assets/tutorial_level/grassy.png") ||
        !textures[AssetType::Button].loadFromFile("assets/tutorial_level/button.png")) {
        return -1;
    }
    
    Background background("assets/tutorial_level/background.png",
                          "assets/tutorial_level/middleground.png",
                          "assets/tutorial_level/mountains.png", sf::Vector2u(1920, 1080));

    std::vector<std::pair<sf::Vector2f, AssetType>> tilePositions = loadLevel(window, platforms, textures, true);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (gameState == GameState::Title) {
                titleScreen.handleInput();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                    if (titleScreen.currentSelection == 0) {
                        gameState = GameState::Play;
                    } else if (titleScreen.currentSelection == 2) {
                        gameState = GameState::Exit;
                        window.close();
                    }
                }
            }

            if (gameState == GameState::Play) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                    currentMode = (currentMode == GameMode::Play) ? GameMode::Edit : GameMode::Play;
                    if (currentMode == GameMode::Edit) {
                        enableMouse();
                    } else {
                        disableMouse();
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D && currentMode == GameMode::Edit) {
                    debugMode = !debugMode;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                    saveLevel(window, tilePositions);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
                    tilePositions = loadLevel(window, platforms, textures, false);
                }

                if (event.key.code == sf::Keyboard::Num1) currentAsset = AssetType::Brick;
                if (event.key.code == sf::Keyboard::Num2) currentAsset = AssetType::Dripstone;
                if (event.key.code == sf::Keyboard::Num3) currentAsset = AssetType::LeftRock;
                if (event.key.code == sf::Keyboard::Num4) currentAsset = AssetType::RightRock;
                if (event.key.code == sf::Keyboard::Num5) currentAsset = AssetType::Tree;
                if (event.key.code == sf::Keyboard::Num6) currentAsset = AssetType::Grassy;
                if (event.key.code == sf::Keyboard::Num7) currentAsset = AssetType::Button;
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
                        sf::Texture tileTexture = textures[currentAsset];
                        sf::Vector2f size(tileTexture.getSize().x, tileTexture.getSize().y);

                        if (currentAsset == AssetType::Grassy) {
                            platforms.emplace_back(tilePos.x, tilePos.y, size.x, size.y, tileTexture, true);
                        } else {
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
                        platforms.erase(platforms.begin() + index);
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
            background.render(window, sf::Vector2u(1920, 1080), playerX, deltaTime);

            for (const auto& tileData : tilePositions) {
                sf::Sprite tile(textures.at(tileData.second));
                tile.setPosition(tileData.first);
                window.draw(tile);
            }

            if (currentMode == GameMode::Play) {
                player->update(deltaTime, platforms, window.getSize().x, window.getSize().y, *enemy);
                enemy->update(deltaTime, platforms, window.getSize().x, window.getSize().y);
                player->draw(window);
                enemy->draw(window);
            }

            if (currentMode == GameMode::Edit && debugMode) {
                drawGrid(window, view.getSize(), gridSize);
            }

            window.display();
        }
    }

    return 0;
}

