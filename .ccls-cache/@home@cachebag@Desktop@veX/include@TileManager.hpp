#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

class TileManager {
public:
    TileManager();
    
    void loadTextures();
    void draw(sf::RenderWindow& window);
    void drawTileSelector(sf::RenderWindow& window);
    void drawTilePreview(sf::RenderWindow& window);
    void drawSaveLoadButtons(sf::RenderWindow& window);
    void handleInput(sf::RenderWindow& window);
    void saveLevel(const std::string& filename);
    void loadLevel(const std::string& filename);
    void toggleDebugMode();
    void openTileSelectorPopup(sf::RenderWindow& mainWindow);  // New function

    const std::vector<sf::RectangleShape>& getPlacedTiles() const;

private:
    enum class TileType { None, Ground, Ground2, Ground3 };
    TileType selectedTile = TileType::Ground;

    sf::Texture groundTexture, ground2Texture, ground3Texture;
    std::map<TileType, sf::Texture*> tileTextures;
    std::vector<sf::RectangleShape> tiles;

    bool debugMode = false;

    const int tileSize = 64;
    std::vector<std::vector<TileType>> levelGrid;

    void drawGrid(sf::RenderWindow& window);
};
