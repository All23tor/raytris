#ifndef RAYTRIS_H
#define RAYTRIS_H

#include "Game.hpp"
#include "Menu.hpp"
#include <raylib.h>

class Raytris {
public:
  Raytris() {
    auto [width, height] = getResolution(Resolutions::SMALL);
    InitWindow(width, height, "RayTris");
    SetTargetFPS(60);
  }

  ~Raytris() { CloseWindow(); }

  void run() {
    Menu().run();
    Game().run();
  }
};

#endif
