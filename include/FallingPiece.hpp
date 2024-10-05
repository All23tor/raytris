#ifndef FALLING_PIECE_H
#define FALLING_PIECE_H

#include "raylib.h"
#include <array>

enum class Tetromino : unsigned char { I, O, T, Z, S, J, L, EMPTY };
enum Orientation : unsigned char { UP, LEFT, DOWN, RIGHT };

struct CoordinatePair {
  signed char x : 4;
  signed char y : 4;
};

using OffsetTable = std::array<CoordinatePair, 5>; // 5 offsets to try
using TetrominoMap =
    std::array<CoordinatePair, 4>; // 4 minos per Tetromino in a 5x5 matrix

Color getTetrominoColor(const Tetromino);
TetrominoMap initialTetrominoMap(const Tetromino);

struct FallingPiece {
  enum Tetromino tetromino;
  enum Orientation orientation;
  char horizontal_position;
  char vertical_position;
  TetrominoMap tetromino_map;

  void fall();
  void shiftLeft();
  void shiftRight();
  void turnClockwise();
  void turnCounterClockwise();

  FallingPiece(Tetromino, char, char);
};

OffsetTable getOffsetTable(const FallingPiece);

#endif
