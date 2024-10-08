cmake_minimum_required(VERSION 3.5.0)
project(raytris)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
        GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(raylib)
endif()

set(SOURCES
    ./src/FallingPiece.cpp
    ./src/Menu.cpp
    ./src/Playfield.cpp
    ./src/NextQueue.cpp
    ./src/Game.cpp
    ./src/Raytris.cpp
    ./src/main.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "include")
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
