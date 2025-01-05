#include "Runnable.hpp"
#include "raylib.h"

void Runnable::run() {
  do {
    BeginDrawing();
    draw();
    EndDrawing();
    update();
  } while (!shouldStopRunning());
}