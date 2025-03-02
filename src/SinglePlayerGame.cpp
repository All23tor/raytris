#include "SinglePlayerGame.hpp"
#include "DrawingDetails.hpp"
#include "HandlingSettings.hpp"
#include "Playfield.hpp"
#include <fstream>

const Controller SinglePlayerGame::keyboardControls{
  []() -> bool {return IsKeyPressed(KEY_R);},
  []() -> bool {return IsKeyPressed(KEY_C);},
  []() -> bool {return IsKeyPressed(KEY_LEFT);},
  []() -> bool {return IsKeyPressed(KEY_RIGHT);},
  []() -> bool {return IsKeyDown(KEY_LEFT);},
  []() -> bool {return IsKeyDown(KEY_RIGHT);},
  []() -> bool {return IsKeyPressed(KEY_UP);},
  []() -> bool {return IsKeyPressed(KEY_Z);},
  []() -> bool {return IsKeyPressed(KEY_A);},
  []() -> bool {return IsKeyPressed(KEY_SPACE);},
  []() -> bool {return IsKeyDown(KEY_DOWN);},
  []() -> bool {return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z);},
  []() -> bool {return IsKeyPressed(KEY_ENTER);},
  []() -> bool {return IsKeyPressed(KEY_ESCAPE);},
};

SinglePlayerGame::SinglePlayerGame(const HandlingSettings& _settings):
  game({DrawingDetails::HeightScaleFactor * GetScreenHeight() / Playfield::VisibleHeight,
      {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}}, keyboardControls, _settings) {
  if (std::ifstream in("save.raytris"); in.good())
    in >> game.playfield; 
  undoMoveStack.push(game.playfield);
}

SinglePlayerGame::~SinglePlayerGame() { 
  std::ofstream out("save.raytris");
  out << game.playfield; 
}

void SinglePlayerGame::update() {
  if (game.controller.checkUndoInput()) {
    if (!undoMoveStack.empty()) {
      game.playfield = undoMoveStack.top();
      undoMoveStack.pop();
      if (undoMoveStack.empty()) undoMoveStack.push(game.playfield);
      return;
    }
  } 
  if (game.update())
    undoMoveStack.push(game.playfield);
}

void SinglePlayerGame::draw() const {
  game.draw();
}

bool SinglePlayerGame::shouldStopRunning() const {
  return game.controller.checkQuitInput() && (game.paused || game.playfield.lost());
}
