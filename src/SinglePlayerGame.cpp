#include "SinglePlayerGame.hpp"
#include "Playfield.hpp"
#include <fstream>

const Controller SinglePlayerGame::KeyboardControls{
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

SinglePlayerGame::SinglePlayerGame():
  Game(KeyboardControls) {
  undoMoveStack.push(playfield);
}

SinglePlayerGame::SinglePlayerGame(const DrawingDetails& drawingDetails) :
  Game(drawingDetails, KeyboardControls) {
  undoMoveStack.push(playfield);
}

void SinglePlayerGame::saveGame() const {
  std::ofstream out("save");
  out << playfield; 
}

void SinglePlayerGame::loadGame() { 
  if (std::ifstream in("save"); in.good()) {
    in >> playfield; 
  }
} 

void SinglePlayerGame::update() {
  if (controller.checkUndoInput()) {
    if (!undoMoveStack.empty()) {
      playfield = undoMoveStack.top();
      undoMoveStack.pop();
      if (undoMoveStack.empty()) undoMoveStack.push(playfield);
      return;
    }
  } else if (controller.checkRestartInput()) {
    playfield.restart();
  }

  if (controller.checkPauseInput()) paused = !paused;
  if (paused) return;
  if (playfield.update(controller)) undoMoveStack.push(playfield);
}

void SinglePlayerGame::DrawPauseMenu() const {
  if (!playfield.lost() && !paused) return;

  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  DrawRectangle(0, 0, screenWidth, screenHeight, DrawingDetails::DarkenColor);

  if (playfield.lost()) {
    DrawText("YOU LOST",
      (screenWidth - MeasureText("YOU LOST", drawingDetails.fontSizeBig)) / 2.0,
      screenHeight / 2.0, drawingDetails.fontSizeBig, drawingDetails.YouLostColor);

  } else if (paused) {
    DrawText("GAME PAUSED",
      (screenWidth - MeasureText("GAME PAUSED", drawingDetails.fontSizeBig)) / 2.0,
      screenHeight / 2.0, drawingDetails.fontSizeBig, drawingDetails.GamePausedColor);
  }
  DrawText("Press Esc to quit",
    (screenWidth - MeasureText("Press Enter to quit", drawingDetails.fontSize)) / 2.0,
    screenHeight / 2.0 + drawingDetails.fontSizeBig, drawingDetails.fontSize, drawingDetails.QuitColor);
}

void SinglePlayerGame::draw() const {
  ClearBackground(DrawingDetails::BackgroundColor);
  playfield.draw(drawingDetails);
  DrawPauseMenu(); 
}
