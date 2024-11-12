#ifndef GAME_H
#define GAME_H

#include "Playfield.hpp"
#include <raylib.h>
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
  Rectangle getBlockRectangle(int, int) const;
  void DrawRectangleRecPretty(Rectangle, Color, Color) const;
  void DrawTetrion() const;
  void update();
  void draw() const;
  void drawPiece(const TetrominoMap &, Color, int, int) const; 

public:
  Game();
  void run();
};

#endif
