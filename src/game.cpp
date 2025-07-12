#include "../hdr/game.hpp"

Game::Game(sf::Vector2u grid_pos, sf::Vector2i grid_size, int cell_size, enum game_type type)
    : game_type(type),
      grid(grid_pos, grid_size, cell_size, 1.0f),
      snake(grid)
{
}

void Game::update(sf::RenderWindow& window, enum direction dir) {
    bool has_eaten_food = snake.update(dir);
    if (has_eaten_food) grid.generate_food();
    grid.renderGrid(window);
}
