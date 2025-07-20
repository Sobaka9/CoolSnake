#include "../hdr/game.hpp"

Game::Game(sf::Vector2i gridSize, sf::Vector2u windowSize, enum gameType type,
    const char* windowName, uint framerate)
    : gameType(type)
    , pause(false)
    , currentDir(LEFT)
    , window(nullptr)
    , grid(sf::Vector2u(0,0), gridSize, 0, 1.0f)
    , snake(grid)
{
    initializeWindow(windowSize, windowName, framerate);

    int cellSize = std::min(
        windowSize.x / gridSize.x,
        windowSize.y / gridSize.y
    );

    sf::Vector2u gridPos = {
        (window->getSize().x - (gridSize.x * cellSize)) / 2,
        (window->getSize().y - (gridSize.y * cellSize)) / 2
    };

    grid = Grid(gridPos, gridSize, cellSize, 1.0f);
    snake = Snake(grid);

    switch (type) {
        case CLASSIC :
            break;
        case HAMILTON :
            computeHamiltonianCycle(grid);
            break;
        case ASTAR :
            initSearch(grid);
            aStarFlood(grid);
            generatePath(grid);
            break;
    }

    // Start the game's loop
    update();
}

void Game::initializeWindow(sf::Vector2u windowSize, const char* windowName, uint framerate) {
    window = new sf::RenderWindow(sf::VideoMode(windowSize), windowName);
    window->setFramerateLimit(framerate);
    window->setPosition(sf::Vector2i(
        (1920 - windowSize.x) / 2 + 1920,
        (1080 - windowSize.y) / 2
    ));
}

void Game::renderGrid(Grid& grid) {

    // Update grid position
    sf::Vector2i gridSize = grid.getSize();
    sf::Vector2u gridPos = {
        (window->getSize().x - gridSize.x) / 2,
        (window->getSize().y - gridSize.y) / 2
    };
    grid.setGridPos(gridPos);

    // Draw each cell
    for(const auto& rowCells : grid.getCells()){
        for(const auto& cell : rowCells){
            cell.drawCell(*window);
        }
    }
}

void Game::renderPath(bool followsHead) {
    int i = 0;

    if (grid.path.empty()) {
        return;
    }

    // Only renders the path ahead of the Snake's head
    if (followsHead) {
        auto it = std::find(grid.path.begin(), grid.path.end(), grid.getHeadCell());
        i = it - grid.path.begin();
    }

    for (i; i < grid.path.size() - 1; i++){
        const Cell* current = grid.path[i];
        const Cell* next = grid.path[(i + 1) % grid.path.size()];
        // Get the vector from the current cell to the next one
        sf::Vector2i segmentDir = next->getPosition() - current->getPosition();
        sf::Vector2f segmentDir_f(
            static_cast<float>(segmentDir.x),
            static_cast<float>(segmentDir.y)
        );
        
        sf::RectangleShape line({segmentDir_f.length(), 1.f});
        line.setPosition(sf::Vector2f(
            static_cast<float>(current->getPosition().x) + segmentDir_f.length() / 2,
            static_cast<float>(current->getPosition().y) + segmentDir_f.length() / 2)
        );

        line.setFillColor(sf::Color(0,0,255,100));
        
        sf::Angle currentAngle = segmentDir_f.angle();
        line.rotate(currentAngle);

        window->draw(line);
    }    
}

void Game::update() {

    while (window->isOpen())
    {
        while (const std::optional event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window->close();
            }

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                pause = !pause;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                currentDir = RIGHT;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                currentDir = LEFT;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                currentDir = DOWN;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                currentDir = UP;
            }

        }

        if (!pause){
            window->clear();
            
            if (gameType != CLASSIC) {
                currentDir = getNextDirectionInPath(snake.getHead(), grid);
            }
            
            bool hasEatenFood = snake.update(currentDir);
            grid.setHeadCell(snake.getHead());

            if (hasEatenFood) {
                grid.generateFood();

                // Restore path and all pathfinding data structures
                clearSearch(grid);
                initSearch(grid);

                switch (gameType) {
                    case CLASSIC :
                        break;
                    case HAMILTON :
                        computeHamiltonianCycle(grid);
                        break;
                    case ASTAR :
                        aStarFlood(grid);
                        generatePath(grid);
                        break;
                }
            }
            
            // Does the path only render ahead of the Snake's head?
            bool followsHead = ((gameType == HAMILTON) ? false : true);
            
            renderPath(followsHead);
            renderGrid(grid);

        }

        window->display();
    }

}
