#ifndef SINGLE_PLAYER_GAME_H
#define SINGLE_PLAYER_GAME_H

#include "Game.hpp"
#include <stack>

class SinglePlayerGame {
  Game game;
  std::stack<Playfield> undoMoveStack;

public:
  SinglePlayerGame(const HandlingSettings&);
  ~SinglePlayerGame();
  void update();
  void draw() const;
  bool should_stop_running() const;
};

#endif
