cmake_minimum_required(VERSION 3.28)
project(cool_snake LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(cool_snake src/main.cpp src/game_objects.cpp src/game.cpp src/path.cpp) 
target_include_directories(cool_snake PUBLIC hdr)
target_compile_features(cool_snake PRIVATE cxx_std_17)
target_link_libraries(cool_snake PRIVATE SFML::Graphics SFML::System)