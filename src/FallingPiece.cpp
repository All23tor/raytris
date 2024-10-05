#include "FallingPiece.hpp"
#include <raylib.h>

FallingPiece::FallingPiece(Tetromino _tetromino, char _horizontal_position,
                           char _vertical_position)
    : tetromino(_tetromino), orientation(Orientation::UP),
      horizontal_position(_horizontal_position),
      vertical_position(_vertical_position),
      tetromino_map(initialTetrominoMap(tetromino)) {}

Color getTetrominoColor(const Tetromino tetromino) {
  switch (tetromino) {
  case Tetromino::I:
    return (Color){49, 199, 239, 255};
  case Tetromino::O:
    return (Color){247, 211, 8, 255};
  case Tetromino::T:
    return (Color){173, 77, 156, 255};
  case Tetromino::S:
    return (Color){66, 182, 66, 255};
  case Tetromino::Z:
    return (Color){239, 32, 41, 255};
  case Tetromino::J:
    return (Color){90, 101, 173, 255};
  case Tetromino::L:
    return (Color){239, 121, 33, 255};
  default:
    return BLANK;
  }
}

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

void FallingPiece::fall() { vertical_position += 1; }

void FallingPiece::shiftLeft() { horizontal_position -= 1; }

void FallingPiece::shiftRight() { horizontal_position += 1; }

void FallingPiece::turnClockwise() {
  for (CoordinatePair &pcp : tetromino_map) {
    pcp = {static_cast<signed char>(-pcp.y), pcp.x};
  }

  switch (orientation) {
  case Orientation::UP:
    orientation = Orientation::RIGHT;
    break;
  case Orientation::RIGHT:
    orientation = Orientation::DOWN;
    break;
  case Orientation::DOWN:
    orientation = Orientation::LEFT;
    break;
  case Orientation::LEFT:
    orientation = Orientation::UP;
    break;
  }
}

void FallingPiece::turnCounterClockwise() {
  for (CoordinatePair &pcp : tetromino_map) {
    pcp = (CoordinatePair){pcp.y, static_cast<signed char>(-pcp.x)};
  }

  switch (orientation) {
  case Orientation::UP:
    orientation = Orientation::LEFT;
    break;
  case Orientation::RIGHT:
    orientation = Orientation::UP;
    break;
  case Orientation::DOWN:
    orientation = Orientation::RIGHT;
    break;
  case Orientation::LEFT:
    orientation = Orientation::DOWN;
    break;
  }
}

OffsetTable getOffsetTable(FallingPiece fallingPiece) {
  switch (fallingPiece.tetromino) {
  case Tetromino::I:
    switch (fallingPiece.orientation) {
    case Orientation::UP:
      return (OffsetTable){{{0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0}}};
    case Orientation::RIGHT:
      return (OffsetTable){{{-1, 0}, {0, 0}, {0, 0}, {0, 1}, {0, -2}}};
    case Orientation::DOWN:
      return (OffsetTable){{{-1, 1}, {1, 1}, {-2, 1}, {1, 0}, {-2, 0}}};
    case Orientation::LEFT:
      return (OffsetTable){{{0, 1}, {0, 1}, {0, 1}, {0, -1}, {0, 2}}};
    }
  case Tetromino::O:
    switch (fallingPiece.orientation) {
    case Orientation::UP:
      return (OffsetTable){{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};
    case Orientation::RIGHT:
      return (OffsetTable){{{0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}}};
    case Orientation::DOWN:
      return (OffsetTable){{{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}}};
    case Orientation::LEFT:
      return (OffsetTable){{{-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}}};
    }
  default:
    switch (fallingPiece.orientation) {
    case Orientation::UP:
      return (OffsetTable){{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};
    case Orientation::RIGHT:
      return (OffsetTable){{{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}};
    case Orientation::DOWN:
      return (OffsetTable){{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};
    case Orientation::LEFT:
      return (OffsetTable){{{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}};
    }
  }
  return {};
}
