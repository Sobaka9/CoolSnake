#include "../hdr/game_objects.hpp"
#include <SFML/Graphics.hpp>

Cell::Cell(sf::Vector2i _position, int _size, int _outlineThickness) {
    pos = _position;
    size = _size;
    outlineThickness = _outlineThickness;
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
    rectangle.setOutlineThickness(outlineThickness);
}

void Cell::drawCell(sf::RenderWindow& window) const {
    window.draw(rectangle);
}

void Cell::setState(enum cellState state) {
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

enum cellState Cell::getState() {
    return state;
}

sf::Vector2i Cell::getPosition() const {
    return pos;
}

Cell* Cell::getNeighbor(enum direction dir) const {
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

///////////////////////////////////// GRID ///////////////////////////////////// 

size_t HashContainer::operator()(const Cell* cell) const{
    sf::Vector2i pos = cell->getPosition();
    // https://stackoverflow.com/questions/45395071/hash-for-a-stdpair-for-use-in-an-unordered-map
    return size_t(pos.x) << 32 | pos.y;
}

QueueItem::QueueItem() : cost(0) {}

QueueItem::QueueItem(const Cell* cell, int cost) : QueueItem() {
    this->cell = cell;
    this->cost = cost;
}

bool QueueItem::operator<(const QueueItem& item) const {
    return cost < item.cost;
}

bool QueueItem::operator>(const QueueItem& item) const {
    return cost > item.cost;
}

void Grid::initializeCellNeighbors() {
    for(int i = 0; i < size.x; i++) {
        for(int j = 0; j < size.y; j++) {
            Cell& currentCell = cells[i][j];
            if (i > 0) 
                currentCell.neighbors.left = &cells[i-1][j];
            if (i < size.x-1) 
                currentCell.neighbors.right = &cells[i+1][j];
            if (j > 0) 
                currentCell.neighbors.up = &cells[i][j-1];
            if (j < size.y-1) 
                currentCell.neighbors.down = &cells[i][j+1];
        }
    }
}

Cell* Grid::getRandomNonSnakeCell() {
    srand(time(0));
    std::vector<Cell*> nonSnakeCells;

    for (auto& row : cells) {
        for (auto& cell : row) {
            if (cell.getState() != SNAKE) {
                nonSnakeCells.push_back(&cell);
            }
        }
    }

    if (nonSnakeCells.empty()) return nullptr;
    int idx = rand() % nonSnakeCells.size();
    return nonSnakeCells[idx];
}

void Grid::generateFood(){
    Cell* randomCell = getRandomNonSnakeCell();
    // WIN
    if(!randomCell){
        exit(EXIT_SUCCESS);
    }
    randomCell->setState(FOOD);
    foodCell = randomCell;
}

Grid::Grid(sf::Vector2u gridPos, sf::Vector2i _gridSize, int cellSize, int _thickness) {
    size = _gridSize;
    pos = gridPos;
    foodCell = nullptr;

    for(int i = 0; i < size.x; i++) {
        std::vector<Cell> rowCells;
        for(int j = 0; j < size.y; j++) {
            sf::Vector2i relativePosition = {i * cellSize, j * cellSize};
            sf::Vector2i globalPosition = sf::Vector2i(
                static_cast<int>(pos.x),
                static_cast<int>(pos.y)
            ) + relativePosition;
            Cell newCell = Cell(globalPosition, cellSize, _thickness);
            rowCells.push_back(newCell);
        }
        cells.push_back(rowCells);
    }
    initializeCellNeighbors();
    generateFood();
}

Cell* Grid::getCell(int cell_x, int cell_y){
    if (cell_x >= size.x or cell_y >= size.y
        or cell_x < 0 or cell_y < 0){
        return NULL;
    }
    return &cells[cell_x][cell_y];
}

const std::vector<std::vector<Cell>>& Grid::getCells() const{
    return cells;
}

sf::Vector2i Grid::getSize() const {
    return size;
}

const Cell* Grid::getFoodCell() const {
    return foodCell;
}

const Cell* Grid::getHeadCell() const {
    return headCell;
}

void Grid::setGridPos(sf::Vector2u pos){
    this->pos = pos;
}

void Grid::setHeadCell(Cell* head) {
    headCell = head;
}

void Grid::setCellState(int cellX, int cellY, enum cellState state) {
    cells[cellX][cellY].setState(state);
}

///////////////////////////////////// SNAKE ///////////////////////////////////// 


Cell* Snake::getHead() const {
    return head;
}

Snake::Snake(Grid& grid) {
    sf::Vector2i midpoint = {grid.getSize().x / 2, grid.getSize().y / 2};

    head = grid.getCell(midpoint.x, midpoint.y);
    head->setState(SNAKE);
    grid.setHeadCell(head);

    snakeCells.push_back(head);
    length = 1;
}

bool Snake::update(enum direction dir) {
    Cell* tail = snakeCells[0];
    bool hasEatenFood = false;

    // move the head according to the input direction
    head = head->getNeighbor(dir);

    if (!head) {
        exit(EXIT_SUCCESS);
    }
    
    switch(head->getState()) {
        case EMPTY:
            // Move Body
            tail->setState(EMPTY);
            for(int i = 0; i < length - 1; i++){
                snakeCells[i] = snakeCells[i+1];
            }
            snakeCells[length-1] = head;
            break;

        case FOOD:
            // Add Food as head and don't move the body
            snakeCells.push_back(head);
            length++;
            hasEatenFood = true;
            break;

        case SNAKE:
            exit(EXIT_SUCCESS);
            break;
    }

    head->setState(SNAKE);
    
    return hasEatenFood;
}
