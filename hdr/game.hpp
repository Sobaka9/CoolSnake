#pragma once

#include "game_objects.hpp"

enum game_type {
    CLASSIC,
    HAMILTON,
}; 

class Game {
    private:
        enum game_type game_type;
        bool pause = false;
        enum direction current_dir = LEFT;
        sf::RenderWindow* window;
        Grid grid;
        Snake snake;
        std::vector<const Cell*> hamiltonian_cycle;
        void compute_hamiltonian_cycle();

    public:
        Game(sf::Vector2i grid_size, sf::Vector2u window_size, enum game_type type, const char* window_name, uint framerate);
        void initialize_window(sf::Vector2u window_size, const char* window_name, uint framerate);
        void update();
        void renderGrid(Grid& grid);
        enum direction get_next_direction_in_cycle(const Cell* current) const;
        void renderPath();
};