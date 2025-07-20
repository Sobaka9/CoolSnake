#pragma once

#include "resources.hpp"

enum cellState {
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
        enum cellState state;
        sf::Vector2i pos;
        sf::RectangleShape rectangle;
        int outlineThickness;
        int size;
        struct {
            Cell* left;
            Cell* right;
            Cell* up;
            Cell* down;
        } neighbors;

    public:
        Cell(sf::Vector2i position, int size, int outlineThickness);
        void drawCell(sf::RenderWindow& window) const;
        void setState(enum cellState);
        enum cellState getState();
        sf::Vector2i getPosition() const;
        Cell* getNeighbor(enum direction dir) const;
        friend class Grid;
};

struct HashContainer {
    std::size_t operator()(const Cell* cell) const;
};

struct QueueItem {
    const Cell* cell;
    float cost;

    QueueItem();
    QueueItem(const Cell* cell, int cost);

    bool operator<(const QueueItem& item) const;
    bool operator>(const QueueItem& item) const;
};


class Grid {
    private:
        sf::Vector2u pos;
        sf::Vector2i size;
        int cellSize;
        Cell* foodCell;
        Cell* headCell;
        std::vector<std::vector<Cell>> cells;
        void initializeCellNeighbors();
        Cell* getRandomNonSnakeCell();
    public:

        std::vector<const Cell*> path;
        std::unordered_set<const Cell*> visited;
        std::priority_queue<QueueItem> priorityFrontier;
        std::unordered_map<const Cell*, const Cell*, HashContainer> pathMap;
        std::unordered_map<const Cell*, float, HashContainer> costMap;

        Grid(sf::Vector2u gridPos, sf::Vector2i gridSize, int cellSize, int thickness);
        void setCellState(int cellX, int cellY, enum cellState state);
        Cell* getCell(int cellX, int cellY);
        const Cell* getFoodCell() const;
        const Cell* getHeadCell() const;
        const std::vector<std::vector<Cell>>& getCells() const;
        sf::Vector2i getSize() const;
        void setGridPos(sf::Vector2u pos);
        void setHeadCell(Cell* head);
        void generateFood();
        
};

class Snake {
    private:
        Cell* head;
        std::vector<Cell*> snakeCells;
        int length;
    public:
        Snake(Grid& grid);
        Cell* getHead() const;
        bool update(enum direction dir);
};
