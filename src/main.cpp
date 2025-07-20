#include "../hdr/game.hpp"

int main()
{
    sf::Vector2i gridSize = {20, 20};
    sf::Vector2 windowSize = {800u, 800u};

    Game game(
        gridSize,
        windowSize,
        HAMILTON,
        "Snake",
        20
    );

}
