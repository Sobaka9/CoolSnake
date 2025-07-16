#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

enum cell_state {
    EMPTY,
    SNAKE,
    FOOD,
};

enum direction {
    LEFT,
    RIGHT,
    DOWN,
    UP,
};

class Cell {
    private:
        enum cell_state state;
        sf::Vector2i pos;
        sf::RectangleShape rectangle;
        int outline_thickness;
        int size;
        struct {
            Cell* left;
            Cell* right;
            Cell* up;
            Cell* down;
        } neighbors;

    public:
        Cell(sf::Vector2i position, int size, int outline_thickness);
        void draw_cell(sf::RenderWindow& window) const;
        void set_state(enum cell_state);
        enum cell_state get_state();
        sf::Vector2i get_position() const;
        Cell* get_neighbor(enum direction dir) const;
        friend class Grid;
};

class Grid {
    private:
        sf::Vector2u pos;
        sf::Vector2i size;
        int cell_size;
        std::vector<std::vector<Cell>> cells;
        void initialize_cell_neighbors();
        Cell* get_random_non_snake_cell();
    public:
        Grid(sf::Vector2u grid_pos, sf::Vector2i grid_size, int cell_size, int thickness);
        void set_cell_state(int cell_x, int cell_y, enum cell_state state);
        Cell* get_cell(int cell_x, int cell_y);
        const std::vector<std::vector<Cell>>& get_cells() const;
        sf::Vector2i get_size() const;
        void set_grid_pos(sf::Vector2u pos);
        void generate_food();
        
};

class Snake {
    private:
        Cell* head;
        std::vector<Cell*> snake_cells;
        int length;
    public:
        Snake(Grid& grid);
        Cell* get_head() const;
        bool update(enum direction dir);
};
