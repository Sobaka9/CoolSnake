#pragma once

#include "game_objects.hpp"

// Manhattan distance
float heuristic(const Cell* head, const Cell* target);
void initSearch(Grid& grid);
void clearSearch(Grid& grid);
bool computeHamiltonianCycle(Grid& grid);
enum direction getNextDirectionInPath(const Cell* current, Grid& grid);
bool aStarFlood(Grid& grid);
void generatePath(Grid& grid);
