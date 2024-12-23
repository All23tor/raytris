#include "Raytris.hpp"
#include "SinglePlayerGame.hpp"
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
    if (exitCode == Menu::ExitCode::SinglePlayerGame) {
      SinglePlayerGame().run();
    } else if (exitCode == Menu::ExitCode::TwoPlayerGame) {
      TwoPlayerGame().run();
    }
  }
}
