#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

class TileManager {
public:
    TileManager();
    
    void loadTextures(); // Loads tile textures
    void draw(sf::RenderWindow& window); // Draw tiles to the screen
    void handleInput(sf::RenderWindow& window); // Handle tile placement
    void saveLevel(const std::string& filename); // Save the current level to a file
    void loadLevel(const std::string& filename); // Load a level from a file
    void toggleDebugMode(); // Toggle debug options like gridlines, etc.

    const std::vector<sf::RectangleShape>& getPlacedTiles() const; // Get the tiles for play mode
    
private:
    enum class TileType { None, Ground, Ground2, Ground3 };
    TileType selectedTile = TileType::Ground;

    sf::Texture groundTexture, ground2Texture, ground3Texture;
    std::map<TileType, sf::Texture*> tileTextures;
    std::vector<sf::RectangleShape> tiles; // Vector to store the placed tiles

    bool debugMode = false;

    const int tileSize = 64; // Tile size in pixels
    std::vector<std::vector<TileType>> levelGrid; // Grid representing the level layout

    void drawGrid(sf::RenderWindow& window); // Draw grid for debugging
};

