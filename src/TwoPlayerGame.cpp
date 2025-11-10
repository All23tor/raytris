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
  []() -> bool { return IsKeyPressed(KEY_R); },
  []() -> bool { return IsKeyPressed(KEY_LEFT_SHIFT); },
  []() -> bool { return IsKeyPressed(KEY_A); },
  []() -> bool { return IsKeyPressed(KEY_D); },
  []() -> bool { return IsKeyDown(KEY_A); },
  []() -> bool { return IsKeyDown(KEY_D); },
  []() -> bool { return IsKeyPressed(KEY_W); },
  []() -> bool { return IsKeyPressed(KEY_Q); },
  []() -> bool { return IsKeyPressed(KEY_E); },
  []() -> bool { return IsKeyPressed(KEY_C); },
  []() -> bool { return IsKeyDown(KEY_S); },
  []() -> bool { return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z); },
  []() -> bool { return IsKeyPressed(KEY_ENTER); },
  []() -> bool { return IsKeyPressed(KEY_ESCAPE); },
};

static constexpr Controller CONTROLS_2{
  []() -> bool { return IsKeyPressed(KEY_R); },
  []() -> bool { return IsKeyPressed(KEY_M); },
  []() -> bool { return IsKeyPressed(KEY_LEFT); },
  []() -> bool { return IsKeyPressed(KEY_RIGHT); },
  []() -> bool { return IsKeyDown(KEY_LEFT); },
  []() -> bool { return IsKeyDown(KEY_RIGHT); },
  []() -> bool { return IsKeyPressed(KEY_UP); },
  []() -> bool { return IsKeyPressed(KEY_B); },
  []() -> bool { return IsKeyPressed(KEY_N); },
  []() -> bool { return IsKeyPressed(KEY_SPACE); },
  []() -> bool { return IsKeyDown(KEY_DOWN); },
  []() -> bool { return false; },
  []() -> bool { return false; },
  []() -> bool { return false; },
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
