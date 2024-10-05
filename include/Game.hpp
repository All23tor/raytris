#ifndef GAME_H
#define GAME_H

#include "Playfield.hpp"
#include <stack>

class Game {
  // Drawing details
  const float block_length;
  const Vector2 position;
  // Playfield
  Playfield playfield;
  // Previous moves
  std::stack<Playfield> undoMoveStack;

private:
  Rectangle getBlockRectangle(int, int);
  void update();
  void draw();

public:
  Game();
  void run();
  void toogleFullScreen();
};

#endif
