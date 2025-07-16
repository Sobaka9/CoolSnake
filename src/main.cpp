#include "../hdr/game.hpp"

int main()
{
    sf::Vector2i grid_size = {20, 20};
    sf::Vector2 window_size = {800u, 800u};

    Game game(
        grid_size,
        window_size,
        HAMILTON,
        "Snake",
        100
    );

}
