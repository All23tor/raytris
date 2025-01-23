#ifndef TWO_PLAYER_GAME_H
#define TWO_PLAYER_GAME_H

#include "Game.hpp"

class TwoPlayerGame: public Game {
  const DrawingDetails drawingDetails2;
  Playfield playfield2;
  // Controller
  const Controller controller2;
  const static Controller FirstPlayerControls;
  const static Controller SecondPlayerControls;

private:
  void DrawPauseMenu() const;
  virtual void update() override final;
  virtual void draw() const override final;

public:
  TwoPlayerGame();
};

#endif
