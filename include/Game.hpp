#ifndef GAME_H
#define GAME_H

#include "Playfield.hpp"
#include <stack>

class Game {
  static constexpr int WINDOWED_WIDTH = 16 * 80;
  static constexpr int WINDOWED_HEIGHT = 720;

  // Drawing details
  float block_length;
  Vector2 position;
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
  ~Game();
  void run();
  void toogleFullScreen();
};

#endif
