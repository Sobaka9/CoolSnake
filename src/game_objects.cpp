#include "../hdr/game_objects.hpp"
#include <SFML/Graphics.hpp>

Cell::Cell(sf::Vector2i _position, int _size, int _outline_thickness) {
    pos = _position;
    size = _size;
    outline_thickness = _outline_thickness;
    state = EMPTY;
    neighbors.left = nullptr;
    neighbors.right = nullptr;
    neighbors.up = nullptr;
    neighbors.down = nullptr;

    sf::RectangleShape square(sf::Vector2f(size, size));
    rectangle = square;
    rectangle.setPosition(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)));
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineColor(sf::Color(100,100,100, 50));
    rectangle.setOutlineThickness(outline_thickness);
}

void Cell::draw_cell(sf::RenderWindow& window) const {
    window.draw(rectangle);
}

void Cell::set_state(enum cell_state state) {
    this->state = state;
    switch (state){
        case EMPTY:
            rectangle.setFillColor(sf::Color::Transparent);
            break;
        case SNAKE:
            rectangle.setFillColor(sf::Color::Green);
            break;
        case FOOD:
            rectangle.setFillColor(sf::Color::Red);
            break;
        default:
            exit(EXIT_FAILURE);
    }
}

enum cell_state Cell::get_state() {
    return state;
}

sf::Vector2i Cell::get_position() const {
    return pos;
}

Cell* Cell::get_neighbor(enum direction dir) const {
    switch(dir) {
        case LEFT:
            return neighbors.left;
        case RIGHT:
            return neighbors.right;
        case UP:
            return neighbors.up;
        case DOWN:
            return neighbors.down;
        default:
            return nullptr;
    }
}

void Grid::initialize_cell_neighbors() {
    for(int i = 0; i < size.x; i++) {
        for(int j = 0; j < size.y; j++) {
            Cell& current_cell = cells[i][j];
            if (i > 0) 
                current_cell.neighbors.left = &cells[i-1][j];
            if (i < size.x-1) 
                current_cell.neighbors.right = &cells[i+1][j];
            if (j > 0) 
                current_cell.neighbors.up = &cells[i][j-1];
            if (j < size.y-1) 
                current_cell.neighbors.down = &cells[i][j+1];
        }
    }
}

Grid::Grid(sf::Vector2u grid_pos, sf::Vector2i _grid_size, int cell_size, int _thickness) {
    size = _grid_size;
    pos = grid_pos;

    for(int i = 0; i < size.x; i++) {
        std::vector<Cell> row_cells;
        for(int j = 0; j < size.y; j++) {
            sf::Vector2i relative_position = {i * cell_size, j * cell_size};
            sf::Vector2i global_position = sf::Vector2i(static_cast<int>(pos.x), static_cast<int>(pos.y)) + relative_position;
            Cell new_cell = Cell(global_position, cell_size, _thickness);
            row_cells.push_back(new_cell);
        }
        cells.push_back(row_cells);
    }
    initialize_cell_neighbors();
    generate_food();
}

void Grid::set_cell_state(int cell_x, int cell_y, enum cell_state state) {
    cells[cell_x][cell_y].set_state(state);
}

Cell* Grid::get_cell(int cell_x, int cell_y){
    if (cell_x >= size.x or cell_y >= size.y
        or cell_x < 0 or cell_y < 0){
        return NULL;
    }
    return &cells[cell_x][cell_y];
}

sf::Vector2i Grid::get_size() const {
    return size;
}

void Grid::generate_food(){
    Cell* random_cell = get_random_non_snake_cell();
    // WIN
    if(!random_cell){
        exit(EXIT_SUCCESS);
    }
    random_cell->set_state(FOOD);
}

Cell* Grid::get_random_non_snake_cell() {
    srand(time(0));
    std::vector<Cell*> non_snake_cells;

    for (auto& row : cells) {
        for (auto& cell : row) {
            if (cell.get_state() != SNAKE) {
                non_snake_cells.push_back(&cell);
            }
        }
    }
    if (non_snake_cells.empty()) return nullptr;
    int idx = rand() % non_snake_cells.size();
    return non_snake_cells[idx];
}


const std::vector<std::vector<Cell>>& Grid::get_cells() const{
    return cells;
}

void Grid::set_grid_pos(sf::Vector2u pos){
    this->pos = pos;
}

Cell* Snake::get_head() const {
    return head;
}

Snake::Snake(Grid& grid) {
    sf::Vector2i midpoint = {grid.get_size().x / 2, grid.get_size().y / 2};
    grid.set_cell_state(midpoint.x, midpoint.y, SNAKE);
    head = grid.get_cell(midpoint.x, midpoint.y);

    snake_cells.push_back(head);
    length = 1;
}

bool Snake::update(enum direction dir) {
    Cell* tail = snake_cells[0];
    bool has_eaten_food = false;

    // move the head according to the input direction
    head = head->get_neighbor(dir);

    if (!head) {
        exit(EXIT_SUCCESS);
    }
    
    switch(head->get_state()) {
        case EMPTY:
            // Move Body
            tail->set_state(EMPTY);
            for(int i = 0; i < length - 1; i++){
                snake_cells[i] = snake_cells[i+1];
            }
            snake_cells[length-1] = head;
            break;

        case FOOD:
            // Add Food as head and don't move the body
            snake_cells.push_back(head);
            length++;
            has_eaten_food = true;
            break;

        case SNAKE:
            exit(EXIT_SUCCESS);
            break;
    }

    head->set_state(SNAKE);
    
    return has_eaten_food;
}
