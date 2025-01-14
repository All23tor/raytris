#ifndef SINGLE_PLAYER_GAME_H
#define SINGLE_PLAYER_GAME_H

#include "Game.hpp"
#include <stack>

class SinglePlayerGame : public Game {
  const static Controller KeyboardControls;
  std::stack<Playfield> undoMoveStack;

private:
  void DrawPauseMenu() const;

public:
  SinglePlayerGame();
  explicit SinglePlayerGame(const DrawingDetails&);
  virtual void update() override;
  virtual void draw() const override;
};

#endif
