#pragma once

#include <SFML/Graphics.hpp>

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
        sf::Vector2f pos;
        sf::RectangleShape rectangle;
        int outline_thickness;
        sf::Vector2f size;

    public:
        Cell(sf::Vector2f position, sf::Vector2f size, int outline_thickness);
        void draw_cell(sf::RenderWindow& window) const;
        void set_state(enum cell_state);
        enum cell_state get_state();
        sf::Vector2f get_position();
};

class Grid {
    private:
        int size;
        int cell_count;
        std::vector<std::vector<Cell>> cells;
        Cell* get_random_non_snake_cell();
    public:
        Grid(int grid_size, int cell_count, int thickness);
        void renderGrid(sf::RenderWindow& window);
        int get_count() const;
        void set_cell_state(int cell_x, int cell_y, enum cell_state state);
        Cell* get_cell(int cell_x, int cell_y);
        int get_size();
        void generate_food();
};

class Snake {
    private:
        Cell* head;
        std::vector<Cell*> snake_cells;
        int length;
    public:
        Snake(Grid& grid);
        void update(Grid& grid, enum direction dir);
};
