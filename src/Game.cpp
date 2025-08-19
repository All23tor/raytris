#include "Game.hpp"
#include "HandlingSettings.hpp"

Game::Game(const DrawingDetails& _drawingDetails, const Controller& _controller,
           const HandlingSettings& _settings) :
  drawingDetails(_drawingDetails),
  controller(_controller),
  settings(_settings) {}

void Game::DrawPauseMenu() const {
  if (!playfield.lost() && !paused)
    return;

  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  DrawRectangle(0, 0, screenWidth, screenHeight, DrawingDetails::DarkenColor);

  if (playfield.lost()) {
    DrawText(
      "YOU LOST",
      (screenWidth - MeasureText("YOU LOST", drawingDetails.fontSizeBig)) / 2.0,
      screenHeight / 2.0, drawingDetails.fontSizeBig,
      drawingDetails.YouLostColor);

  } else if (paused) {
    DrawText(
      "GAME PAUSED",
      (screenWidth - MeasureText("GAME PAUSED", drawingDetails.fontSizeBig)) /
        2.0,
      screenHeight / 2.0, drawingDetails.fontSizeBig,
      drawingDetails.GamePausedColor);
  }
  DrawText("Press Esc to quit",
           (screenWidth -
            MeasureText("Press Enter to quit", drawingDetails.fontSize)) /
             2.0,
           screenHeight / 2.0 + drawingDetails.fontSizeBig,
           drawingDetails.fontSize, drawingDetails.QuitColor);
}

void Game::draw() const {
  playfield.draw(drawingDetails);
  DrawPauseMenu();
}

bool Game::update() {
  if (controller.checkRestartInput()) {
    playfield.restart();
  }

  if (controller.checkPauseInput())
    paused = !paused;
  if (paused)
    return false;
  return playfield.update(controller, settings);
}
