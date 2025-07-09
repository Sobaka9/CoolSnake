#include "../hdr/game_objects.hpp"
#include <SFML/Graphics.hpp>

Cell::Cell(sf::Vector2f _position, sf::Vector2f _size, int _outline_thickness) {
    pos = _position;
    size = _size;
    outline_thickness = _outline_thickness;
    state = EMPTY;

    sf::RectangleShape square(size);
    rectangle = square;
    rectangle.setPosition(pos);
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineColor(sf::Color(100,100,100));
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

sf::Vector2f Cell::get_position() {
    return pos;
}

Grid::Grid(int _grid_size, int _cell_count, int _thickness) {
    size = _grid_size;
    cell_count = _cell_count;
    for(int i = 0; i < cell_count; i++) {
        std::vector<Cell> row_cells;
        for(int j = 0; j < cell_count; j++) {
            sf::Vector2f _position = {static_cast<float>(i) / _cell_count * _grid_size, static_cast<float>(j) / _cell_count * _grid_size};
            sf::Vector2f _size = {static_cast<float>(_grid_size) / _cell_count, static_cast<float>(_grid_size) / _cell_count};
            Cell new_cell = Cell(_position, _size, _thickness);
            row_cells.push_back(new_cell);
        }
        cells.push_back(row_cells);
    }
    generate_food();
}

void Grid::renderGrid(sf::RenderWindow& window) {
    for(const auto& row_cells : cells){
        for(const auto& cell : row_cells){
            cell.draw_cell(window);
        }
    }
}

int Grid::get_count() const{
    return cell_count;
}

void Grid::set_cell_state(int cell_x, int cell_y, enum cell_state state) {
    cells[cell_x * cell_count/size][cell_y * cell_count/size].set_state(state);
}

Cell* Grid::get_cell(int cell_x, int cell_y){
    if (cell_x >= size or cell_y >= size
        or cell_x < 0 or cell_y < 0){
        return NULL;
    }
    return &cells[cell_x * cell_count/size][cell_y * cell_count/size];
}

int Grid::get_size(){
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

Snake::Snake(Grid& grid) {
    int midpoint = grid.get_size() / 2;
    grid.set_cell_state(midpoint, midpoint, SNAKE);
    head = grid.get_cell(midpoint, midpoint);
    snake_cells.push_back(head);
    length = 1;
}

void Snake::update(Grid& grid, enum direction dir) {
    sf::Vector2f head_pos = head->get_position();
    int offset = grid.get_size() / grid.get_count();
    Cell* tail = snake_cells[0];

    switch (dir) {
        case LEFT:
            head = grid.get_cell(head_pos.x - offset, head_pos.y);
            break;
        case RIGHT:
            head = grid.get_cell(head_pos.x + offset, head_pos.y);
            break;
        case DOWN:
            head = grid.get_cell(head_pos.x, head_pos.y + offset);
            break;
        case UP:
            head = grid.get_cell(head_pos.x, head_pos.y - offset);
            break;
    }

    if (!head) {
        exit(EXIT_SUCCESS);
    }
    
    switch(head->get_state()) {
        case EMPTY:
            // Move Body
            grid.set_cell_state(tail->get_position().x, tail->get_position().y, EMPTY);
            for(int i = 0; i < length - 1; i++){
                snake_cells[i] = snake_cells[i+1];
            }
            snake_cells[length-1] = head;
            break;

        case FOOD:
            // Add Food as head and don't move the body
            snake_cells.push_back(head);
            length++;
            grid.generate_food();
            break;

        case SNAKE:
            exit(EXIT_SUCCESS);
            break;
    }

    grid.set_cell_state(head->get_position().x, head->get_position().y, SNAKE);
}
