
#include "Raytris.hpp"
#include "Game.hpp"
#include "Menu.hpp"
#include <raylib.h>

Raytris::Raytris() {
  InitWindow(640, 480, "RayTris");
  SetTargetFPS(60);
}

Raytris::~Raytris() { CloseWindow(); }

void Raytris::run() {
  Menu menu;
  while (menu.run()) {
    Game().run();
  }
}
