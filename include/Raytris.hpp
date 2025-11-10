#ifndef RAYTRIS_H
#define RAYTRIS_H

#include "MainMenu.hpp"
#include "SettingsMenu.hpp"
#include "SinglePlayerGame.hpp"
#include "TwoPlayerGame.hpp"
#include <variant>

class Raytris {
  std::variant<MainMenu, SinglePlayerGame, TwoPlayerGame, SettingsMenu> raytris;
  bool should_stop_running = false;

  void handle_stop_runnig(auto&&);

public:
  Raytris();
  ~Raytris();
  void run();
};

#endif
