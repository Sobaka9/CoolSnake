#include "../hdr/path.hpp"

const Cell* focusCell;

float heuristic(const Cell* base, const Cell* target) {
    return static_cast<float>(
        std::abs(base->getPosition().x - target->getPosition().x) +
        std::abs(base->getPosition().y - target->getPosition().y)
    );
}

void initSearch(Grid& grid) {
    clearSearch(grid); 

    grid.priorityFrontier.push(QueueItem(grid.getHeadCell(), 0)); 
    //grid.visited.insert(grid.getHeadCell()); 
    grid.pathMap[grid.getHeadCell()] = grid.getHeadCell();
    grid.costMap[grid.getHeadCell()] = 0; 
}

void clearSearch(Grid& grid) {
    // Function to clear the frontier queues. 
    static auto clearFrontiers = [&]() {
        while (!grid.priorityFrontier.empty())
            grid.priorityFrontier.pop(); 
    };

    clearFrontiers(); 
    grid.visited.clear(); 
    grid.pathMap.clear(); 
    grid.costMap.clear(); 
    grid.path.clear();
}


bool computeHamiltonianCycle(Grid& grid) {
    grid.path.clear();
    // gridCells is a constant reference to the cells of the grid
    const std::vector<std::vector<Cell>>& gridCells = grid.getCells();
    sf::Vector2i gridSize = grid.getSize();

    for (int j = 0; j < gridSize.y; j++){
        if (j % 2 == 0) {
            // Move right
            for (int i = 1; i < gridSize.x; i++) {
                grid.path.push_back(&gridCells[i][j]);
            }
        }
        else{
            // Move left
            for (int i = gridSize.x - 1; i >= 1; i--) {
                grid.path.push_back(&gridCells[i][j]);
            }
        }
    }
    // Connect last row back to start
    if (gridSize.y % 2 == 0) {
        // If even number of rows, we're at left side
        for (int j = gridSize.y - 1; j >= 0; j--) {
            grid.path.push_back(&gridCells[0][j]);
        }
    }
}

enum direction getNextDirectionInPath(const Cell* current, Grid& grid) {
    for (size_t i = 0; i < grid.path.size(); i++) {
        if (grid.path[i] == current) {
            const Cell* nextCell = grid.path[(i + 1) % grid.path.size()];
            for (auto dir : {LEFT, RIGHT, UP, DOWN}){
                if (nextCell == current->getNeighbor(dir)){
                    return dir;
                }
            }
        }
    }
    return LEFT;
}

// https://github.com/OfficialCodeNoodles/Visualizing-Pathfinding-Algorithms/blob/main/src/path.cpp
bool aStarFlood(Grid& grid) {

    while (!grid.priorityFrontier.empty()) {

        // Retrieve the best candidate from the PQ
        const Cell* currentCell = grid.priorityFrontier.top().cell;
        grid.priorityFrontier.pop();

        // Skip if we've already visited this cell
        if (grid.visited.find(currentCell) != grid.visited.end()) {
            continue;
        }

        focusCell = currentCell;
        grid.visited.insert(currentCell);

        // Exit early if a path has been found
        if (currentCell == grid.getFoodCell()){
            return true;
        }

        Cell* nextCells[4];
        int neigborCount = 0;

        // Store all valid neighbors
        for (auto& dir : {LEFT, RIGHT, UP, DOWN}){
            Cell* testingCell = currentCell->getNeighbor(dir);

            if (testingCell && testingCell->getState() != SNAKE
                && grid.visited.find(testingCell) == grid.visited.end()) {
                nextCells[neigborCount] = testingCell;
                neigborCount++;
            }

        }

        for (int nextIndex = 0; nextIndex < neigborCount; nextIndex++) {
            Cell* nextCell = nextCells[nextIndex];
            float cost = grid.costMap[currentCell] + 1.0f;

            // Push the neighboring cell in the PQ
            if (grid.costMap.count(nextCell) == 0 || cost < grid.costMap[nextCell]) {
                grid.priorityFrontier.push(QueueItem(
                    nextCell,
                    -cost + -heuristic(grid.getFoodCell(), nextCell) * 1.4f 
                ));
            }
                // Create a link between the next position and the previous
                grid.pathMap[nextCell] = currentCell;
                grid.costMap[nextCell] = cost;
        }

    }
    return true;
}

void generatePath(Grid& grid) {
    const Cell* currentCell = grid.getFoodCell();
    const Cell* previousCell = nullptr;

    grid.path.clear();

    if (grid.pathMap.count(currentCell) != 0) {
        while (currentCell != grid.getHeadCell()){
            grid.path.push_back(currentCell);

            currentCell = grid.pathMap[currentCell];

            if (previousCell == currentCell) {
                grid.path.clear();
                return;
            }

            previousCell = currentCell;
        }

        grid.path.push_back(currentCell);
    }
    
    // Reverse the path
    std::reverse(grid.path.begin(), grid.path.end());
}
