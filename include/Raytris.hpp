#ifndef RAYTRIS_H
#define RAYTRIS_H

#include "MainMenu.hpp"
#include "SinglePlayerGame.hpp"
#include "TwoPlayerGame.hpp"
#include "SettingsMenu.hpp"
#include <variant>

class Raytris {
  std::variant<MainMenu, SinglePlayerGame, TwoPlayerGame, SettingsMenu> raytris;
  bool shouldStopRunning = false;

  void handleWhereToGo(auto&&);
  void updateDrawFrame();
public:
  Raytris();
  ~Raytris();
  void run();
};

#endif
