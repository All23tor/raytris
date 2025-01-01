#include <emscripten/emscripten.h>
#include "SinglePlayerGame.hpp"

namespace RaylibWeb {
  constexpr int windowWidth = 800, windowHeight = 450;

  namespace {
    SinglePlayerGame singlePlayerGame{ {
      DrawingDetails::HeightScaleFactor * windowHeight / (Playfield::VisibleHeight),
      {windowWidth / 2.0f, windowHeight / 2.0f}
    } };
  }

  void UpdateDrawFrame() {
    singlePlayerGame.update();
    BeginDrawing();
    ClearBackground(DrawingDetails::BackgroundColor);
    singlePlayerGame.draw();
    EndDrawing();
  }
}


int main() {
  InitWindow(RaylibWeb::windowWidth, RaylibWeb::windowHeight, "Raytris");
  emscripten_set_main_loop(RaylibWeb::UpdateDrawFrame, 0, 1);
  CloseWindow();
}

