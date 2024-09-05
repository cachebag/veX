#ifndef ORB_HPP
#define ORB_HPP
#include <SFML/Graphics.hpp>

class Orb {
    public:
        Orb(float x, float y, float radius);

        void draw(sf::RenderWindow &window) const;

        bool isCollected(sf::FloatRect playerBounds);

        bool hasBeenCollected() const;

    private:
        sf::CircleShape shape;
        bool collected;
};

#endif
