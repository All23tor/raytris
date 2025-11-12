#include "FallingPiece.hpp"
#include <utility>

FallingPiece::FallingPiece(
  Tetromino _tetromino, char _horizontal_position, char _vertical_position
) :
  tetromino(_tetromino),
  orientation(Orientation::Up),
  x(_horizontal_position),
  y(_vertical_position),
  map(initial_tetromino_map(tetromino)) {}

TetrominoMap initial_tetromino_map(Tetromino tetromino) {
  switch (tetromino) {
  case Tetromino::I:
    return (TetrominoMap){{{-1, 0}, {0, 0}, {1, 0}, {2, 0}}};
  case Tetromino::O:
    return (TetrominoMap){{{0, -1}, {1, -1}, {0, 0}, {1, 0}}};
  case Tetromino::T:
    return (TetrominoMap){{{0, -1}, {-1, 0}, {0, 0}, {1, 0}}};
  case Tetromino::S:
    return (TetrominoMap){{{0, -1}, {1, -1}, {-1, 0}, {0, 0}}};
  case Tetromino::Z:
    return (TetrominoMap){{{-1, -1}, {0, -1}, {0, 0}, {1, 0}}};
  case Tetromino::J:
    return (TetrominoMap){{{-1, -1}, {-1, 0}, {0, 0}, {1, 0}}};
  case Tetromino::L:
    return (TetrominoMap){{{1, -1}, {-1, 0}, {0, 0}, {1, 0}}};
  default:
    return (TetrominoMap){{{0, 0}, {0, 0}, {0, 0}, {0, 0}}};
  }
}

void FallingPiece::fall() {
  y += 1;
}

void FallingPiece::shift(Shift shift) {
  if (shift == Shift::Left)
    x -= 1;
  else if (shift == Shift::Right)
    x += 1;
}

void FallingPiece::rotate(RotationType rotationType) {
  for (CoordinatePair& pcp : map)
    if (rotationType == RotationType::Clockwise)
      pcp = {static_cast<signed char>(-pcp.y), pcp.x};
    else if (rotationType == RotationType::CounterClockwise)
      pcp = {pcp.y, static_cast<signed char>(-pcp.x)};
    else if (rotationType == RotationType::OneEighty)
      pcp = {
        static_cast<signed char>(-pcp.x), static_cast<signed char>(-pcp.y)
      };

  int rotationSteps = (rotationType == RotationType::Clockwise) ? 1 :
    (rotationType == RotationType::CounterClockwise)            ? 3 :
    (rotationType == RotationType::OneEighty)                   ? 2 :
                                                                  0;
  orientation = static_cast<Orientation>(
    (std::to_underlying(orientation) + rotationSteps) % 4
  );
}

void FallingPiece::translate(CoordinatePair translation) {
  x += translation.x;
  y += translation.y;
}

FallingPiece FallingPiece::fallen() const {
  FallingPiece new_piece = *this;
  new_piece.fall();
  return new_piece;
}

FallingPiece FallingPiece::shifted(Shift shiftDirection) const {
  FallingPiece new_piece = *this;
  new_piece.shift(shiftDirection);
  return new_piece;
}

FallingPiece FallingPiece::rotated(RotationType rotationType) const {
  FallingPiece new_piece = *this;
  new_piece.rotate(rotationType);
  return new_piece;
}

FallingPiece FallingPiece::translated(CoordinatePair translation) const {
  FallingPiece new_piece = *this;
  new_piece.translate(translation);
  return new_piece;
}

const OffsetTable& get_offset_table(const FallingPiece& piece) {
  static constexpr OffsetTable I_TABLE[4] = {
    {{{0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0}}},
    {{{-1, 0}, {0, 0}, {0, 0}, {0, -1}, {0, 2}}},
    {{{-1, -1}, {1, -1}, {-2, -1}, {1, 0}, {-2, 0}}},
    {{{0, -1}, {0, -1}, {0, -1}, {0, 1}, {0, -2}}},
  };
  static constexpr OffsetTable O_TABLE[4] = {
    {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
    {{{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}}},
    {{{-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}}},
    {{{-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}}},
  };
  static constexpr OffsetTable DEFAULT_TABLE[4]{
    {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
    {{{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}}},
    {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
    {{{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}},
  };

  const auto& offset_table = piece.tetromino == Tetromino::I ? I_TABLE :
    piece.tetromino == Tetromino::O                          ? O_TABLE :
                                                               DEFAULT_TABLE;
  return offset_table[std::to_underlying(piece.orientation)];
}
