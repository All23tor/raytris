#ifndef GAME_H
#define GAME_H

#include "Playfield.hpp"
#include <stack>

class Game {
  // Drawing details
  const float blockLength;
  const Vector2 position;
  // Playfield
  Playfield playfield;
  bool paused = false;
  // Previous moves
  std::stack<Playfield> undoMoveStack;

private:
  Rectangle getBlockRectangle(int, int);
  void update();
  void draw();

public:
  Game();
  void run();
};

#endif
