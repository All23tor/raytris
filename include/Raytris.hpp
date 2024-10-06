#ifndef RAYTRIS_H
#define RAYTRIS_H

#include "Game.hpp"
#include "Menu.hpp"
#include <raylib.h>

class Raytris {
public:
  Raytris() {
    InitWindow(640, 480, "RayTris");
    SetTargetFPS(60);
  }

  ~Raytris() { CloseWindow(); }

  void run() {
    Menu().run();
    Game().run();
  }
};

#endif
