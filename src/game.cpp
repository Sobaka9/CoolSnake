#include "../hdr/game.hpp"

Game::Game(enum game_type type)
    : game_type(type),
      grid(800, 20, 1.0f),
      snake(grid)
{
}

void Game::update(sf::RenderWindow& window, enum direction dir) {
    snake.update(grid, dir);
    grid.renderGrid(window);
}
