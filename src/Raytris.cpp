#include "Raytris.hpp"
#include "Game.hpp"
#include "TwoPlayerGame.hpp"
#include "Menu.hpp"
#include <raylib.h>

Raytris::Raytris() {
  InitWindow(Menu::InitialWidth, Menu::InitialHeight, "RayTris");
  SetTargetFPS(60);
}

Raytris::~Raytris() {
  CloseWindow();
}

void Raytris::run() {
  Menu menu;
  for (Menu::ExitCode exitCode = menu.run(); exitCode != Menu::ExitCode::Exit; exitCode = menu.run()) {
    if (exitCode == Menu::ExitCode::Game) {
      TwoPlayerGame().run();
    }
  }
}
