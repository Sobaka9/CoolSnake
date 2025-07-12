#include "../hdr/game_objects.hpp"
#include "../hdr/game.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    sf::Vector2i grid_size = {20, 20};
    sf::Vector2 window_size = {800u, 800u};

    int cell_size = std::min(
        window_size.x / grid_size.x,
        window_size.y / grid_size.y
    );

    auto window = sf::RenderWindow(sf::VideoMode(window_size), "Snake");
    window.setFramerateLimit(10);
    window.setPosition(sf::Vector2i((1920 - window_size.x)/2 + 1920, (1080 - window_size.y)/2));

    // Center the grid
    sf::Vector2u grid_pos = {
        (window.getSize().x - (grid_size.x * cell_size)) / 2,
        (window.getSize().y - (grid_size.y * cell_size)) / 2
    };

    Game game(grid_pos, grid_size, cell_size, CLASSIC);

    bool pause = false;
    enum direction current_dir = LEFT;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                pause = !pause;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                current_dir = RIGHT;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                current_dir = LEFT;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                current_dir = DOWN;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                current_dir = UP;
            }

        }

        if (!pause){
            window.clear();
            game.update(window, current_dir);
        }

        window.display();

    }
}
