#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
// #include <memory> soon 

class LevelEditor {
public:
  LevelEditor(float gridSize, const std::vector<sf::Texture> &textures);

  void run();
  void drawGrid(sf::RenderWindow& window);
  void handleInput(sf::RenderWindow& window);
  void saveLevel(const std::string& filename);
  void loadLevel(const std::string& filename);

private:
  float gridSize;
  std::vector<sf::Texture> textures;
  std::vector<sf::RectangleShape> placedTiles;
  sf::Texture* selectedTexture;
  sf::RectangleShape previewTile;
};
