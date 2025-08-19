#include "TwoPlayerGame.hpp"
#include "HandlingSettings.hpp"

TwoPlayerGame::TwoPlayerGame(const HandlingSettings& _settings1,
                             const HandlingSettings& _settings2) :
    game1({DrawingDetails::HeightScaleFactor * 0.75f * GetScreenHeight() /
               Playfield::VisibleHeight,
           {GetScreenWidth() / 4.0f, GetScreenHeight() / 2.0f}},
          FirstPlayerControls, _settings1),
    game2({DrawingDetails::HeightScaleFactor * 0.75f * GetScreenHeight() /
               Playfield::VisibleHeight,
           {GetScreenWidth() * 3.0f / 4.0f, GetScreenHeight() / 2.0f}},
          SecondPlayerControls, _settings2) {}

void TwoPlayerGame::update() {
  game1.update();
  game2.update();
  game2.paused = game1.paused;
}

void TwoPlayerGame::draw() const {
  game1.draw();
  game2.draw();
}

bool TwoPlayerGame::shouldStopRunning() const {
  return game1.controller.checkQuitInput() &&
         (game1.paused || game1.playfield.lost());
}
