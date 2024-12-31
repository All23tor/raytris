#include "Playfield.hpp"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

namespace {
  constexpr int windowWidth = 1600, windowHeight = 900;
  Playfield playfield{};
  Controller controller{
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
  DrawingDetails drawingDetails{
    DrawingDetails::HeightScaleFactor * windowHeight / (Playfield::VisibleHeight),
    {windowWidth / 2.0f, windowHeight / 2.0f}
  };
}

void UpdateDrawFrame(){
  playfield.update(controller);
  BeginDrawing();
  ClearBackground(DrawingDetails::BackgroundColor);
  playfield.draw(drawingDetails);
  EndDrawing();
}

int main(void) {
  InitWindow(windowWidth, windowHeight, "Raytris");
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif
  CloseWindow();
  return 0;
}

