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

set(COMMON_SOURCES
    ./src/FallingPiece.cpp
    ./src/Menu.cpp
    ./src/DrawingDetails.cpp
    ./src/Playfield.cpp
    ./src/NextQueue.cpp
    ./src/Controller.cpp
    ./src/Game.cpp
    ./src/SinglePlayerGame.cpp
    ./src/TwoPlayerGame.cpp
    ./src/Raytris.cpp
)

set(DESKTOP_SOURCES
  ./main.cpp
)

set(WEB_SOURCES
  ./main-web.cpp
)

if (${PLATFORM} STREQUAL "Web")
  add_executable(${PROJECT_NAME} ${COMMON_SOURCES} ${WEB_SOURCES})
else()
  add_executable(${PROJECT_NAME} ${COMMON_SOURCES} ${DESKTOP_SOURCES})
endif()

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
