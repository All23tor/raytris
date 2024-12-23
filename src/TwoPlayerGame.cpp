#include "TwoPlayerGame.hpp"

const Controller TwoPlayerGame::KeyboardControls2{
  []() -> bool {return IsKeyPressed(KEY_R);},
  []() -> bool {return IsKeyPressed(KEY_LEFT_SHIFT);},
  []() -> bool {return IsKeyPressed(KEY_A);},
  []() -> bool {return IsKeyPressed(KEY_D);},
  []() -> bool {return IsKeyDown(KEY_A);},
  []() -> bool {return IsKeyDown(KEY_D);},
  []() -> bool {return IsKeyPressed(KEY_RIGHT);},
  []() -> bool {return IsKeyPressed(KEY_LEFT);},
  []() -> bool {return IsKeyPressed(KEY_UP);},
  []() -> bool {return IsKeyPressed(KEY_SPACE);},
  []() -> bool {return IsKeyDown(KEY_DOWN);},
  []() -> bool {return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z);},
  []() -> bool {return IsKeyPressed(KEY_ENTER);},
  []() -> bool {return IsKeyPressed(KEY_ESCAPE);},
};

TwoPlayerGame::TwoPlayerGame() :
  drawingDetails2(
    DrawingDetails::HeightScaleFactor * 0.75 * GetScreenHeight() / (Playfield::VisibleHeight),
    {GetScreenWidth() * 3.0f / 4.0f, GetScreenHeight() / 2.0f}
  ),
  controller2(KeyboardControls2) {}

void TwoPlayerGame::update() {
  if (controller.checkPauseInput() || this->controller2.checkPauseInput()) paused = !paused;
  if (paused) return;
  playfield.update(controller);
  playfield2.update(controller2);
}

void TwoPlayerGame::DrawPauseMenu() const {
  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  if (playfield.lost() || paused) {
    DrawRectangle(0, 0, screenWidth/2, screenHeight, DrawingDetails::DarkenColor);
    if (playfield.lost()) {
        DrawText("YOU LOST",
        (screenWidth/2 - MeasureText("YOU LOST", drawingDetails.fontSize)) / 2.0,
        screenHeight / 2.0, drawingDetails.fontSize, drawingDetails.YouLostColor);

    } else if (paused) {
        DrawText("GAME PAUSED",
        (screenWidth/2 - MeasureText("GAME PAUSED", drawingDetails.fontSize)) / 2.0,
        screenHeight / 2.0, drawingDetails.fontSize, drawingDetails.GamePausedColor);
    }
    DrawText("Press Esc to quit",
        (screenWidth/2 - MeasureText("Press Enter to quit", drawingDetails.fontSize)) / 2.0,
        screenHeight / 2.0 + drawingDetails.fontSize, drawingDetails.fontSize, drawingDetails.QuitColor);
  }

  if (playfield2.lost() || paused) {
    DrawRectangle(screenWidth/2, 0, screenWidth/2, screenHeight, DrawingDetails::DarkenColor);
    if (playfield2.lost()) {
        DrawText("YOU LOST",
        (3*screenWidth/2 - MeasureText("YOU LOST", drawingDetails.fontSize)) / 2.0,
        screenHeight / 2.0, drawingDetails.fontSize, drawingDetails.YouLostColor);

    } else if (paused) {
        DrawText("GAME PAUSED",
        (3*screenWidth/2 - MeasureText("GAME PAUSED", drawingDetails.fontSize)) / 2.0,
        screenHeight / 2.0, drawingDetails.fontSize, drawingDetails.GamePausedColor);
    }
    DrawText("Press Esc to quit",
        (3*screenWidth/2 - MeasureText("Press Enter to quit", drawingDetails.fontSize)) / 2.0,
        screenHeight / 2.0 + drawingDetails.fontSize, drawingDetails.fontSize, drawingDetails.QuitColor);
  }
}

void TwoPlayerGame::draw() const {
  ClearBackground(DrawingDetails::BackgroundColor);
  playfield.draw(drawingDetails);
  playfield2.draw(drawingDetails2);
  DrawPauseMenu();
}