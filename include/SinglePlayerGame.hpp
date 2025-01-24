#ifndef SINGLE_PLAYER_GAME_H
#define SINGLE_PLAYER_GAME_H

#include "Game.hpp"
#include <stack>

class SinglePlayerGame {
  const static Controller keyboardControls;
  Game game;
  std::stack<Playfield> undoMoveStack;

public:
  SinglePlayerGame(const HandlingSettings&);
  ~SinglePlayerGame();
  void update();
  void draw() const;
  bool shouldStopRunning() const;
};

#endif
