#ifndef GAME_H
#define GAME_H

#include "Playfield.hpp"
#include <raylib.h>
#include <stack>

class Game {
  // Drawing details
  static constexpr int LEFT_BORDER = -10;
  const float blockLength;
  const int fontSize;
  const int fontSizeBig;
  const int fontSizeSmall;
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
  void DrawPieces() const;
  void DrawNextComingPieces() const;
  void DrawHoldPiece() const;
  void DrawLineClearMessage() const;
  void DrawCombo() const;
  void DrawBackToBack() const;
  void DrawScore() const;
  void DrawPauseMenu() const;
  void update();
  void draw() const;
  void drawPiece(const TetrominoMap &, Color, int, int) const; 

public:
  Game();
  void run();
};

#endif
