#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>

const int defaultTileDrawSize = 64;

enum class TileType {
    None,
    Brick,
    BushLarge,
    BushSmall,
    SlimeBlock,
    Statue,
    Stone1,
    Stone2,
    Stone3,
    Stone4,
    Tree1,
    Tree2,
    Tree3,
    Rock
};

struct TileProperties {
    bool isSolid;
    sf::Vector2f size;
    bool canBeResized;
    TileProperties(bool solid = true, const sf::Vector2f& size = {defaultTileDrawSize, defaultTileDrawSize}, bool resize = false)
        : isSolid(solid), size(size), canBeResized(resize) {}
};

class Tile {
public:
    TileType type;
    sf::RectangleShape shape;
    TileProperties properties;

    Tile(TileType type, const sf::Vector2f& position, const sf::Texture& texture, const TileProperties& properties);
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

    bool loadLevelFromFile(const std::string& filePath);
    const std::vector<Tile>& getPlacedTiles() const;
    const std::map<TileType, sf::Texture>& getTileTextures() const;
    void setSelectedTile(TileType type);  // New function to set selected tile from Sidebar

private:
    TileType selectedTile = TileType::Brick;

    std::map<TileType, sf::Texture> tileTextures;  // Store textures for each tile type
    std::map<TileType, TileProperties> tileProperties;  // Store properties for each tile type
    std::vector<Tile> tiles;  // Store placed tiles

    bool debugMode = false;
    const int tileSize = defaultTileDrawSize;
    std::vector<std::vector<TileType>> levelGrid;

    void drawGrid(sf::RenderWindow& window);
    void setupTileProperties();  // Setup properties for different tiles
};

