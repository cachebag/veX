#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>

const int tileDrawSize = 64;

enum class TileType { None, Brick, BushLarge, BushSmall, SlimeBlock, Statue, Stone1, Stone2, Stone3, Stone4, Tree1, Tree2, Tree3, Rock };

class Tile {
public:
    TileType type;
    sf::RectangleShape shape;
    Tile(TileType type, const sf::Vector2f& position, const sf::Texture& texture);
};

class TileManager {
public:
    TileManager();

    void loadTextures();
    void draw(sf::RenderWindow& window);
    void drawTilePreview(sf::RenderWindow& window);
    void handleInput(sf::RenderWindow& window);
    void saveLevel();
    void loadLevel();
    void toggleDebugMode();
    void drawSidebar(sf::RenderWindow& window);

    const std::vector<Tile>& getPlacedTiles() const;
    const std::map<TileType, sf::Texture>& getTileTextures() const;

private:
    TileType selectedTile = TileType::Brick;

    std::map<TileType, sf::Texture> tileTextures;  // Store textures for each tile type
    std::vector<Tile> tiles;  // Store placed tiles

    bool debugMode = false;
    const int tileSize = 64;
    std::vector<std::vector<TileType>> levelGrid;

    bool isMouseInSidebar(sf::RenderWindow& window);
    void drawGrid(sf::RenderWindow& window);
};

