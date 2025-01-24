#include "TwoPlayerGame.hpp"
#include "HandlingSettings.hpp"

const Controller TwoPlayerGame::FirstPlayerControls{
  []() -> bool {return IsKeyPressed(KEY_R);},
  []() -> bool {return IsKeyPressed(KEY_LEFT_SHIFT);},
  []() -> bool {return IsKeyPressed(KEY_A);},
  []() -> bool {return IsKeyPressed(KEY_D);},
  []() -> bool {return IsKeyDown(KEY_A);},
  []() -> bool {return IsKeyDown(KEY_D);},
  []() -> bool {return IsKeyPressed(KEY_W);},
  []() -> bool {return IsKeyPressed(KEY_Q);},
  []() -> bool {return IsKeyPressed(KEY_E);},
  []() -> bool {return IsKeyPressed(KEY_C);},
  []() -> bool {return IsKeyDown(KEY_S);},
  []() -> bool {return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z);},
  []() -> bool {return IsKeyPressed(KEY_ENTER);},
  []() -> bool {return IsKeyPressed(KEY_ESCAPE);},
};

const Controller TwoPlayerGame::SecondPlayerControls{
  []() -> bool {return IsKeyPressed(KEY_R);},
  []() -> bool {return IsKeyPressed(KEY_M);},
  []() -> bool {return IsKeyPressed(KEY_LEFT);},
  []() -> bool {return IsKeyPressed(KEY_RIGHT);},
  []() -> bool {return IsKeyDown(KEY_LEFT);},
  []() -> bool {return IsKeyDown(KEY_RIGHT);},
  []() -> bool {return IsKeyPressed(KEY_UP);},
  []() -> bool {return IsKeyPressed(KEY_B);},
  []() -> bool {return IsKeyPressed(KEY_N);},
  []() -> bool {return IsKeyPressed(KEY_SPACE);},
  []() -> bool {return IsKeyDown(KEY_DOWN);},
  []() -> bool {return false;},
  []() -> bool {return false;},
  []() -> bool {return false;},
};

TwoPlayerGame::TwoPlayerGame(const HandlingSettings& _settings1, const HandlingSettings& _settings2) :
  game1({
    DrawingDetails::HeightScaleFactor * 0.75f * GetScreenHeight() / Playfield::VisibleHeight,
        {GetScreenWidth() / 4.0f, GetScreenHeight() / 2.0f}},
    FirstPlayerControls,
    _settings1
  ),
  game2({DrawingDetails::HeightScaleFactor * 0.75f * GetScreenHeight() / Playfield::VisibleHeight,
    {GetScreenWidth() * 3.0f / 4.0f, GetScreenHeight() / 2.0f}}, SecondPlayerControls, _settings2) 
{}  

void TwoPlayerGame::update() {
  game1.update();
  game2.update();
}

void TwoPlayerGame::draw() const {
  game1.draw();
  game2.draw();
}

bool TwoPlayerGame::shouldStopRunning() const {
  bool stop1 = game1.controller.checkQuitInput() && (game1.paused || game1.playfield.lost());
  bool stop2 = game2.controller.checkQuitInput() && (game2.paused || game2.playfield.lost());
  return stop1 || stop2;
}
