#include "../include/Orb.hpp"

Orb::Orb(float x, float y, float radius)
    : collected(false) 
{
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(x, y);
}

void Orb::draw(sf::RenderWindow &window) {
    if (!collected) {
        window.draw(shape);
    }
}

bool Orb::isCollected(sf::FloatRect playerBounds) {
    if (!collected && shape.getGlobalBounds().intersects(playerBounds)) {
        collected = true;
        return true;
    }
    return false;
}

bool Orb::hasBeenCollected() const {
    return collected;
}

