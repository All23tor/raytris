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
  for (Menu::Option exitCode = menu.run(); exitCode != Menu::Option::Exit; exitCode = menu.run()) {
    if (exitCode == Menu::Option::SinglePlayer) {
      SinglePlayerGame().run();
    } else if (exitCode == Menu::Option::TwoPlayers) {
      TwoPlayerGame().run();
    }
  }
}
