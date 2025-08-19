cmake_minimum_required(VERSION 3.11)
project(raytris)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)


set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  endif()
endif()

set(SOURCES
  ./src/FallingPiece.cpp
  ./src/MainMenu.cpp
  ./src/SettingsMenu.cpp
  ./src/Playfield.cpp
  ./src/NextQueue.cpp
  ./src/Game.cpp
  ./src/SinglePlayerGame.cpp
  ./src/TwoPlayerGame.cpp
  ./src/Raytris.cpp
  ./main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME} PRIVATE "include")
target_link_libraries(${PROJECT_NAME} raylib)

if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
endif()

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
