#include "FallingPiece.hpp"
#include <utility>

FallingPiece::FallingPiece(Tetromino _tetromino, char _horizontal_position,
                           char _vertical_position) :
  tetromino(_tetromino),
  orientation(Orientation::Up),
  horizontalPosition(_horizontal_position),
  verticalPosition(_vertical_position),
  tetrominoMap(initialTetrominoMap(tetromino)) {}

TetrominoMap initialTetrominoMap(Tetromino tetromino) {
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

FallingPiece& FallingPiece::fall() {
  verticalPosition += 1;
  return *this;
}

FallingPiece& FallingPiece::shift(Shift shift) {
  if (shift == Shift::Left) {
    horizontalPosition -= 1;
  } else if (shift == Shift::Right) {
    horizontalPosition += 1;
  }

  return *this;
}

FallingPiece& FallingPiece::rotate(RotationType rotationType) {
  for (CoordinatePair& pcp : tetrominoMap)
    if (rotationType == RotationType::Clockwise)
      pcp = {static_cast<signed char>(-pcp.y), pcp.x};
    else if (rotationType == RotationType::CounterClockwise)
      pcp = {pcp.y, static_cast<signed char>(-pcp.x)};
    else if (rotationType == RotationType::OneEighty)
      pcp = {static_cast<signed char>(-pcp.x),
             static_cast<signed char>(-pcp.y)};

  int rotationSteps = (rotationType == RotationType::Clockwise)          ? 1
                      : (rotationType == RotationType::CounterClockwise) ? 3
                      : (rotationType == RotationType::OneEighty)        ? 2
                                                                         : 0;
  orientation = static_cast<Orientation>(
    (std::to_underlying(orientation) + rotationSteps) % 4);

  return *this;
}

FallingPiece& FallingPiece::translate(CoordinatePair translation) {
  horizontalPosition += translation.x;
  verticalPosition += translation.y;

  return *this;
}

FallingPiece FallingPiece::fallen() const {
  FallingPiece newPiece = *this;
  newPiece.fall();
  return newPiece;
}

FallingPiece FallingPiece::shifted(Shift shiftDirection) const {
  FallingPiece newPiece = *this;
  newPiece.shift(shiftDirection);
  return newPiece;
}

FallingPiece FallingPiece::rotated(RotationType rotationType) const {
  FallingPiece newPiece = *this;
  newPiece.rotate(rotationType);
  return newPiece;
}

FallingPiece FallingPiece::translated(CoordinatePair translation) const {
  FallingPiece newPiece = *this;
  newPiece.translate(translation);
  return newPiece;
}

const OffsetTable& getOffsetTable(const FallingPiece& piece) {
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

  const auto& offset_table = piece.tetromino == Tetromino::I   ? I_TABLE
                             : piece.tetromino == Tetromino::O ? O_TABLE
                                                               : DEFAULT_TABLE;
  return offset_table[std::to_underlying(piece.orientation)];
}
