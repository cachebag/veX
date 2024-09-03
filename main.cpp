#include <SFML/Graphics.hpp>
#include <vector>

class Platform {
public:
    sf::RectangleShape shape;

    Platform(float x, float y, float width, float height, sf::Color color = sf::Color::Green) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
    }

    sf::FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(shape);
    }
};

class Player {
public:
    float x, y;
    float yVelocity;
    const float gravity = 75.0f;
    const float terminalVelocity = 10.0f;
    const float speedX = 400.0f;
    const float jumpVelocity = -20.0f;
    sf::RectangleShape shape;

    Player(float startX = 0.0f, float startY = 500.0f) 
        : x(startX), y(startY), yVelocity(0.0f) 
    {
        shape.setSize(sf::Vector2f(50.0f, 50.0f));
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(x, y);
    }

    void update(float deltaTime, const std::vector<Platform>& platforms) {
        handleInput(deltaTime);
        applyGravity(deltaTime);
        move(deltaTime, platforms);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(shape);
    }

private:
    void handleInput(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            x -= speedX * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            x += speedX * deltaTime;
        }
        shape.setPosition(x, y);
    }

    void applyGravity(float deltaTime) {
        yVelocity += gravity * deltaTime;
        if (yVelocity > terminalVelocity) {
            yVelocity = terminalVelocity;
        }
    }

    void move(float deltaTime, const std::vector<Platform>& platforms) {
        y += yVelocity;
        sf::FloatRect playerBounds = shape.getGlobalBounds();
        playerBounds.top = y;

        bool onGround = false;

        for (const auto& platform : platforms) {
            if (playerBounds.intersects(platform.getBounds())) {
                sf::FloatRect platformBounds = platform.getBounds();
                if (yVelocity > 0.0f && (playerBounds.top + playerBounds.height) > platformBounds.top) {
                    y = platformBounds.top - playerBounds.height;
                    yVelocity = 0.0f;
                    onGround = true;
                }
            }
        }

        shape.setPosition(x, y);

        if (onGround && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            yVelocity = jumpVelocity;
        }
    }
};

int main() 
{	
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "veX");
    window.setFramerateLimit(60);

    Player player(0, 1000);

    std::vector<Platform> platforms;
    platforms.emplace_back(0.0f, 1030.0f, 1920.0f, 50.0f);
    platforms.emplace_back(300.0f, 800.0f, 200.0f, 20.0f);
    platforms.emplace_back(600.0f, 600.0f, 200.0f, 20.0f);
    platforms.emplace_back(900.0f, 400.0f, 200.0f, 20.0f);

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        player.update(deltaTime, platforms);

        window.clear();

        for (const auto& platform : platforms) {
            platform.draw(window);
        }

        player.draw(window);

        window.display();
    }
    return 0;
}

