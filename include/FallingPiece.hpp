#ifndef FALLING_PIECE_H
#define FALLING_PIECE_H

#include <array>

enum class Tetromino : unsigned char {
  I,
  O,
  T,
  Z,
  S,
  J,
  L,
  Empty
};
enum class Orientation : unsigned char {
  Up,
  Right,
  Down,
  Left
};
enum class Shift : unsigned char {
  Left,
  Right
};
enum class RotationType : unsigned char {
  Clockwise,
  CounterClockwise,
  OneEighty
};

struct CoordinatePair {
  signed char x : 4;
  signed char y : 4;
};

// 5 offsets to try
using OffsetTable = std::array<CoordinatePair, 5>;
// 4 minos per Tetromino in a 5x5 matrix
using TetrominoMap = std::array<CoordinatePair, 4>;

TetrominoMap initial_tetromino_map(const Tetromino);

struct FallingPiece {
  Tetromino tetromino;
  Orientation orientation;
  char x_position;
  char y_position;
  TetrominoMap tetromino_map;

  FallingPiece(Tetromino, char, char);
  void fall();
  void shift(Shift);
  void rotate(RotationType);
  void translate(CoordinatePair);
  FallingPiece fallen() const;
  FallingPiece shifted(Shift) const;
  FallingPiece rotated(RotationType) const;
  FallingPiece translated(CoordinatePair translation) const;
};

const OffsetTable& get_offset_table(const FallingPiece&);

#endif
