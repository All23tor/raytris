#ifndef TWO_PLAYER_GAME_H
#define TWO_PLAYER_GAME_H

#include "Game.hpp"
#include "HandlingSettings.hpp"

class TwoPlayerGame {
  Game game1;
  Game game2;

  void DrawPauseMenu() const;

public:
  TwoPlayerGame(const HandlingSettings&, const HandlingSettings&);
  void update();
  void draw() const;
  bool shouldStopRunning() const;
};

#endif
