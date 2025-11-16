#include "TwoPlayerGame.hpp"
#include "HandlingSettings.hpp"

static DrawingDetails makeDrawingDetails1() {
  float blockLength{
    DrawingDetails::HEIGHT_SCALE_FACTOR * 0.75f * GetScreenHeight() /
    Playfield::VISIBLE_HEIGHT
  };
  Vector2 position{
    GetScreenWidth() / 4.0f - blockLength * Playfield::WIDTH / 2.0f,
    GetScreenHeight() / 2.0f - blockLength * Playfield::VISIBLE_HEIGHT / 2.0f
  };
  return {blockLength, position};
};

static DrawingDetails makeDrawingDetails2() {
  float blockLength{
    DrawingDetails::HEIGHT_SCALE_FACTOR * 0.75f * GetScreenHeight() /
    Playfield::VISIBLE_HEIGHT
  };
  Vector2 position{
    (GetScreenWidth() * 3.0f / 2.0f - blockLength * Playfield::WIDTH) / 2.0f,
    (GetScreenHeight() - blockLength * Playfield::VISIBLE_HEIGHT) / 2.0f
  };
  return {blockLength, position};
};

static constexpr Controller CONTROLS_1{
  []() -> bool { return false; },
  []() -> bool { return IsKeyPressed(KEY_E); },
  []() -> bool { return IsKeyPressed(KEY_A); },
  []() -> bool { return IsKeyPressed(KEY_D); },
  []() -> bool { return IsKeyDown(KEY_A); },
  []() -> bool { return IsKeyDown(KEY_D); },
  []() -> bool { return IsKeyPressed(KEY_W); },
  []() -> bool { return IsKeyPressed(KEY_Q); },
  []() -> bool { return IsKeyPressed(KEY_R); },
  []() -> bool { return IsKeyPressed(KEY_Z); },
  []() -> bool { return IsKeyDown(KEY_S); },
  []() -> bool { return false; },
  []() -> bool { return IsKeyPressed(KEY_ENTER); },
  []() -> bool { return IsKeyPressed(KEY_ESCAPE); },
};

static constexpr Controller CONTROLS_2{
  []() -> bool { return false; },
  []() -> bool { return IsKeyPressed(KEY_O); },
  []() -> bool { return IsKeyPressed(KEY_J); },
  []() -> bool { return IsKeyPressed(KEY_L); },
  []() -> bool { return IsKeyDown(KEY_J); },
  []() -> bool { return IsKeyDown(KEY_L); },
  []() -> bool { return IsKeyPressed(KEY_I); },
  []() -> bool { return IsKeyPressed(KEY_U); },
  []() -> bool { return IsKeyPressed(KEY_P); },
  []() -> bool { return IsKeyPressed(KEY_M); },
  []() -> bool { return IsKeyDown(KEY_K); },
  []() -> bool { return false; },
  []() -> bool { return IsKeyPressed(KEY_ENTER); },
  []() -> bool { return IsKeyPressed(KEY_ESCAPE); },
};

TwoPlayerGame::TwoPlayerGame(
  const HandlingSettings& settings1, const HandlingSettings& settings2
) :
  game1(makeDrawingDetails1(), CONTROLS_1, settings1),
  game2(makeDrawingDetails2(), CONTROLS_2, settings2) {}

void TwoPlayerGame::update() {
  game1.update();
  game2.update();
  game2.paused = game1.paused;
}

void TwoPlayerGame::draw() const {
  game1.draw();
  game2.draw();
}

bool TwoPlayerGame::should_stop_running() const {
  return game1.controller.quit() && (game1.paused || game1.playfield.lost());
}
