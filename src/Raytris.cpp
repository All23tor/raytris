#include "Raytris.hpp"
#include "Game.hpp"
#include "Menu.hpp"
#include <raylib.h>

Raytris::Raytris() {
  InitWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "RayTris");
  SetTargetFPS(60);
}

Raytris::~Raytris() {
  CloseWindow();
}

void Raytris::run() {
  Menu menu;
  while (menu.run() == Menu::ExitCode::Game) {
    Game().run();
  }
}
