#pragma once

#include "game_objects.hpp"
#include "path.hpp"

enum gameType {
    CLASSIC,
    HAMILTON,
    ASTAR,
}; 

class Game {
    private:
        enum gameType gameType;
        bool pause = false;
        enum direction currentDir = LEFT;
        sf::RenderWindow* window;
        Grid grid;
        Snake snake;

    public:
        Game(sf::Vector2i gridSize, sf::Vector2u windowSize, enum gameType type, const char* windowName, uint framerate);
        void initializeWindow(sf::Vector2u windowSize, const char* windowName, uint framerate);
        void update();
        void renderGrid(Grid& grid);
        void renderPath(bool followsHead);
};
