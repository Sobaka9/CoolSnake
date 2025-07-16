#include "../hdr/game.hpp"

Game::Game(sf::Vector2i grid_size, sf::Vector2u window_size, enum game_type type,
    const char* window_name, uint framerate)
    : game_type(type)
    , pause(false)
    , current_dir(LEFT)
    , window(nullptr)
    , grid(sf::Vector2u(0,0), grid_size, 0, 1.0f)
    , snake(grid)
{
    initialize_window(window_size, window_name, framerate);

    int cell_size = std::min(
        window_size.x / grid_size.x,
        window_size.y / grid_size.y
    );

    sf::Vector2u grid_pos = {
        (window->getSize().x - (grid_size.x * cell_size)) / 2,
        (window->getSize().y - (grid_size.y * cell_size)) / 2
    };

    grid = Grid(grid_pos, grid_size, cell_size, 1.0f);
    snake = Snake(grid);

    compute_hamiltonian_cycle();
    // Starts the game's loop
    update();
}

void Game::initialize_window(sf::Vector2u window_size, const char* window_name, uint framerate) {
    window = new sf::RenderWindow(sf::VideoMode(window_size), window_name);
    window->setFramerateLimit(framerate);
    window->setPosition(sf::Vector2i(
        (1920 - window_size.x) / 2 + 1920,
        (1080 - window_size.y) / 2
    ));
}

void Game::renderGrid(Grid& grid) {

    // Update grid position
    sf::Vector2i grid_size = grid.get_size();
    sf::Vector2u grid_pos = {
        (window->getSize().x - grid_size.x) / 2,
        (window->getSize().y - grid_size.y) / 2
    };
    grid.set_grid_pos(grid_pos);

    // Draw each cell
    for(const auto& row_cells : grid.get_cells()){
        for(const auto& cell : row_cells){
            cell.draw_cell(*window);
        }
    }
}

void Game::renderPath(){
    for (int i = 0; i < hamiltonian_cycle.size(); i++){
        const Cell* current = hamiltonian_cycle[i];
        const Cell* next = hamiltonian_cycle[(i + 1) % hamiltonian_cycle.size()];
        sf::Vector2i segment_dir = next->get_position() - current->get_position();
        
        sf::Vector2f segment_dir_f(
            static_cast<float>(segment_dir.x),
             static_cast<float>(segment_dir.y)
        );
        
        sf::RectangleShape line({segment_dir_f.length(), 1.f});
        line.setPosition(sf::Vector2f(
            static_cast<float>(current->get_position().x) + segment_dir_f.length() / 2,
            static_cast<float>(current->get_position().y) + segment_dir_f.length() / 2)
        );

        line.setFillColor(sf::Color(0,0,255,100));
        
        sf::Angle current_angle = segment_dir_f.angle();
        line.rotate(current_angle);

        window->draw(line);
    }    
}
void Game::compute_hamiltonian_cycle() {
    hamiltonian_cycle.clear();
    // grid_cells is a constant reference to the cells of the grid
    const std::vector<std::vector<Cell>>& grid_cells = grid.get_cells();
    sf::Vector2i grid_size = grid.get_size();

    for (int j = 0; j < grid_size.y; j++){
        if (j % 2 == 0) {
            // Move right
            for (int i = 1; i < grid_size.x; i++) {
                hamiltonian_cycle.push_back(&grid_cells[i][j]);
            }
        }
        else{
            // Move left
            for (int i = grid_size.x - 1; i >= 1; i--) {
                hamiltonian_cycle.push_back(&grid_cells[i][j]);
            }
        }
    }
    // Connect last row back to start
    if (grid_size.y % 2 == 0) {
        // If even number of rows, we're at left side
        for (int j = grid_size.y - 1; j >= 0; j--) {
            hamiltonian_cycle.push_back(&grid_cells[0][j]);
        }
    }
}

enum direction Game::get_next_direction_in_cycle(const Cell* current) const {
    for (size_t i = 0; i < hamiltonian_cycle.size(); i++) {
        if (hamiltonian_cycle[i] == current) {
            const Cell* next_cell = hamiltonian_cycle[(i + 1) % hamiltonian_cycle.size()];
            for (auto dir : {LEFT, RIGHT, UP, DOWN}){
                if (next_cell == current->get_neighbor(dir)){
                    return dir;
                }
            }
        }
    }
    return LEFT;
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
                current_dir = RIGHT;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                current_dir = LEFT;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                current_dir = DOWN;

                if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                current_dir = UP;
            }

        }

        if (!pause){
            window->clear();

            renderPath();
            renderGrid(grid);
            
            if (game_type == HAMILTON) {
                current_dir = get_next_direction_in_cycle(snake.get_head());
            }
            
            bool has_eaten_food = snake.update(current_dir);
            if (has_eaten_food) grid.generate_food();

        }

        window->display();
    }

}
