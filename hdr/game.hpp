#pragma once

#include "game_objects.hpp"

enum game_type {
    CLASSIC,
}; 

class Game {
    private:
        enum game_type game_type;
        Grid grid;
        Snake snake;

    public:
        Game(sf::Vector2u grid_pos, sf::Vector2i grid_size, int cell_size, enum game_type type);
        void update(sf::RenderWindow& window, enum direction dir);
        
};