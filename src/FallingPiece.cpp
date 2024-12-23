#include "FallingPiece.hpp"

FallingPiece::FallingPiece(Tetromino _tetromino, char _horizontal_position, char _vertical_position)
  : tetromino(_tetromino), orientation(Orientation::Up),
  horizontalPosition(_horizontal_position),
  verticalPosition(_vertical_position),
  tetrominoMap(initialTetrominoMap(tetromino)) {}

TetrominoMap initialTetrominoMap(Tetromino tetromino) {
  switch (tetromino) {
  case Tetromino::I:
    return (TetrominoMap) { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } } };
  case Tetromino::O:
    return (TetrominoMap) { { { 0, -1 }, { 1, -1 }, { 0, 0 }, { 1, 0 } } };
  case Tetromino::T:
    return (TetrominoMap) { { { 0, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 } } };
  case Tetromino::S:
    return (TetrominoMap) { { { 0, -1 }, { 1, -1 }, { -1, 0 }, { 0, 0 } } };
  case Tetromino::Z:
    return (TetrominoMap) { { { -1, -1 }, { 0, -1 }, { 0, 0 }, { 1, 0 } } };
  case Tetromino::J:
    return (TetrominoMap) { { { -1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 } } };
  case Tetromino::L:
    return (TetrominoMap) { { { 1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 } } };
  default:
    return (TetrominoMap) { { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
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
  for (CoordinatePair& pcp : tetrominoMap) {
    if (rotationType == RotationType::Clockwise) {
      pcp = { static_cast<signed char>(-pcp.y), pcp.x };
    } else if (rotationType == RotationType::CounterClockwise) {
      pcp = { pcp.y, static_cast<signed char>(-pcp.x) };
    } else if (rotationType == RotationType::OneEighty) {
      pcp = { static_cast<signed char>(-pcp.x), static_cast<signed char>(-pcp.y) };
    }
  }

  int rotationSteps =
    (rotationType == RotationType::Clockwise) ? 1
    : (rotationType == RotationType::CounterClockwise) ? -1
    : (rotationType == RotationType::OneEighty) ? 2
    : 0;
  orientation = static_cast<Orientation>((static_cast<unsigned char>(orientation) + rotationSteps + 4) % 4);

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

OffsetTable getOffsetTable(const FallingPiece& fallingPiece) {
  switch (fallingPiece.tetromino) {
  case Tetromino::I:
    switch (fallingPiece.orientation) {
    case Orientation::Up:
      return (OffsetTable) { { { 0, 0 }, { -1, 0 }, { 2, 0 }, { -1, 0 }, { 2, 0 } } };
    case Orientation::Right:
      return (OffsetTable) { { { -1, 0 }, { 0, 0 }, { 0, 0 }, { 0, -1 }, { 0, 2 } } };
    case Orientation::Down:
      return (OffsetTable) { { { -1, -1 }, { 1, -1 }, { -2, -1 }, { 1, 0 }, { -2, 0 } } };
    case Orientation::Left:
      return (OffsetTable) { { { 0, -1 }, { 0, -1 }, { 0, -1 }, { 0, 1 }, { 0, -2 } } };
    }
  case Tetromino::O:
    switch (fallingPiece.orientation) {
    case Orientation::Up:
      return (OffsetTable) { { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
    case Orientation::Right:
      return (OffsetTable) { { { 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 } } };
    case Orientation::Down:
      return (OffsetTable) { { { -1, 1 }, { -1, 1 }, { -1, 1 }, { -1, 1 }, { -1, 1 } } };
    case Orientation::Left:
      return (OffsetTable) { { { -1, 0 }, { -1, 0 }, { -1, 0 }, { -1, 0 }, { -1, 0 } } };
    }
  default:
    switch (fallingPiece.orientation) {
    case Orientation::Up:
      return (OffsetTable) { { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
    case Orientation::Right:
      return (OffsetTable) { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 } } };
    case Orientation::Down:
      return (OffsetTable) { { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
    case Orientation::Left:
      return (OffsetTable) { { { 0, 0 }, { -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 } } };
    }
  }
  return {};
}
