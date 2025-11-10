#include "Game.hpp"
#include "HandlingSettings.hpp"

Game::Game(
  const DrawingDetails& _drawing_details,
  const Controller& _controller,
  const HandlingSettings& _settings
) :
  drawing_details(_drawing_details),
  controller(_controller),
  settings(_settings) {}

void Game::draw() const {
  playfield.draw(drawing_details);

  // Pause Menu
  if (!playfield.lost() && !paused)
    return;

  const float width = GetScreenWidth();
  const float height = GetScreenHeight();
  DrawRectangle(0, 0, width, height, DrawingDetails::DARKEN_COLOR);

  if (playfield.lost()) {
    DrawText(
      "YOU LOST",
      (width - MeasureText("YOU LOST", drawing_details.font_size_big)) / 2.0,
      height / 2.0,
      drawing_details.font_size_big,
      drawing_details.YOU_LOST_COLOR
    );

  } else if (paused) {
    DrawText(
      "GAME PAUSED",
      (width - MeasureText("GAME PAUSED", drawing_details.font_size_big)) / 2.0,
      height / 2.0,
      drawing_details.font_size_big,
      drawing_details.GAME_PAUSED_COLOR
    );
  }
  DrawText(
    "Press Esc to quit",
    (width - MeasureText("Press Enter to quit", drawing_details.font_size)) /
      2.0,
    height / 2.0 + drawing_details.font_size_big,
    drawing_details.font_size,
    drawing_details.QUIT_COLOR
  );
}

bool Game::update() {
  if (controller.restart())
    playfield.restart();

  if (controller.pause())
    paused = !paused;
  if (paused)
    return false;
  return playfield.update(controller, settings);
}
