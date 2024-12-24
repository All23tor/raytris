#include "Game.hpp"

Game::Game() :
  drawingDetails(
    DrawingDetails::HeightScaleFactor * GetScreenHeight() / (Playfield::VisibleHeight),
    {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}
  ),
  controller(KeyboardControls) {}

Game::Game(const DrawingDetails& _drawingDetails) :
  drawingDetails(_drawingDetails),
  controller(KeyboardControls) {}

Game::Game(const Controller& _controller) :
  drawingDetails(
    DrawingDetails::HeightScaleFactor * GetScreenHeight() / (Playfield::VisibleHeight),
    {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}
  ),
  controller(_controller) {}

Game::Game(const DrawingDetails& _drawingDetails, const Controller& _controller) :
  drawingDetails(_drawingDetails),
  controller(_controller) {}

void Game::run() {
  while (!controller.checkQuitInput() || !(paused || playfield.lost())) {
    update();
    BeginDrawing();
    draw();
    EndDrawing();
  }

  BeginDrawing();
  EndDrawing();
}