#ifndef SINGLE_PLAYER_GAME_H
#define SINGLE_PLAYER_GAME_H

#include "Game.hpp"
#include <stack>

class SinglePlayerGame : public Game {
  const static Controller KeyboardControls;
  std::stack<Playfield> undoMoveStack;

private:
  void DrawPauseMenu() const;
  virtual void update() override;
  virtual void draw() const override;

public:
  SinglePlayerGame();
};

#endif
