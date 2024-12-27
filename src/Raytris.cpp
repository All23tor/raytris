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
  for (Menu::Option selectedOption = menu.run(); selectedOption != Menu::Option::Exit; selectedOption = menu.run()) {
    if (selectedOption == Menu::Option::SinglePlayer) {
      SinglePlayerGame().run();
    } else if (selectedOption == Menu::Option::TwoPlayers) {
      TwoPlayerGame().run();
    }
  }
}
