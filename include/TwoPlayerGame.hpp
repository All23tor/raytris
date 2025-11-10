#ifndef TWO_PLAYER_GAME_H
#define TWO_PLAYER_GAME_H

#include "Game.hpp"

class TwoPlayerGame {
  Game game1;
  Game game2;

public:
  TwoPlayerGame(const HandlingSettings&, const HandlingSettings&);
  void update();
  void draw() const;
  bool should_stop_running() const;
};

#endif
