#ifndef RAYTRIS_H
#define RAYTRIS_H

#include "SinglePlayerGame.hpp"
#include "TwoPlayerGame.hpp"
#include "Menu.hpp"
#include <variant>

class Raytris {
  Menu menu;
  std::variant<Menu*, SinglePlayerGame, TwoPlayerGame> raytris = &menu;
  bool shouldStopRunning = false;

  void handleWhereToGo(auto&&);
  void updateDrawFrame();
public:
  Raytris();
  ~Raytris();
  void run();
};

#endif
