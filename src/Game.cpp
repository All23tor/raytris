#include "Game.hpp"

Game::Game(const Controller& _controller) :
  drawingDetails(
    DrawingDetails::HeightScaleFactor * GetScreenHeight() / (Playfield::VisibleHeight),
    {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}
  ),
  controller(_controller) {}

Game::Game(const DrawingDetails& _drawingDetails, const Controller& _controller) :
  drawingDetails(_drawingDetails),
  controller(_controller) {}

bool Game::shouldStopRunning() const {
  return controller.checkQuitInput() && (paused || playfield.lost());
}