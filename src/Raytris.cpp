#include "Raytris.hpp"
#include "Game.hpp"
#include "Menu.hpp"
#include <raylib.h>

Raytris::Raytris() {
  InitWindow(Menu::INITIAL_WIDTH, Menu::INITIAL_HEIGHT, "RayTris");
  SetTargetFPS(60);
}

Raytris::~Raytris() {
  CloseWindow();
}

void Raytris::run() {
  Menu menu;
  for (Menu::ExitCode exitCode = menu.run(); exitCode != Menu::ExitCode::Exit; exitCode = menu.run()) {
    if (exitCode == Menu::ExitCode::Game) {
      Game().run();
    }
  }
}
