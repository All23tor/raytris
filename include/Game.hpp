#ifndef GAME_H
#define GAME_H

#include "Playfield.hpp"
#include <raylib.h>
#include <stack>

class Game {
  const DrawingDetails drawingDetails;
  Playfield playfield;
  bool paused = false;
  // Previous moves
  std::stack<Playfield> undoMoveStack;
  // Controller
  const Controller controller;
  const static Controller KeyboardControls;
  const static Controller KeyboardControls2;

private:
  void update();
  void DrawPauseMenu() const;
  void draw() const;

public:
  Game();
  void run();
};

#endif
