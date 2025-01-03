#include "Runnable.hpp"
#include "raylib.h"

void Runnable::run() {
  do {
    BeginDrawing();
    draw();
    EndDrawing();
  } while (update(), shouldStopRunning());
}