#include "raylib.h"
#include "Playfield.hpp"
#include <algorithm>
#include <functional>

namespace {
  std::function<bool()> checkRestartInput = []() -> bool {return IsKeyPressed(KEY_R);};
  std::function<bool()> checkSwapInput = []() -> bool {return IsKeyPressed(KEY_C);};
  std::function<bool()> checkLeftInput = []() -> bool {return IsKeyPressed(KEY_LEFT);};
  std::function<bool()> checkRightInput = []() -> bool {return IsKeyPressed(KEY_RIGHT);};
  std::function<bool()> checkLeftDasInput = []() -> bool {return IsKeyDown(KEY_LEFT);};
  std::function<bool()> checkRightDasInput = []() -> bool {return IsKeyDown(KEY_RIGHT);};
  std::function<bool()> checkClockwiseInput = []() -> bool {return IsKeyPressed(KEY_UP);};
  std::function<bool()> checkCounterClockwiseInput = []() -> bool {return IsKeyPressed(KEY_Z);};
  std::function<bool()> checkOneEightyInput = []() -> bool {return IsKeyPressed(KEY_A);};
  std::function<bool()> checkHardDropInput = []() -> bool {return IsKeyPressed(KEY_SPACE);};
  std::function<bool()> checkSoftDropInput = []() -> bool {return IsKeyDown(KEY_DOWN);};
}

Playfield::Playfield() : fallingPiece(Tetromino::Empty, INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION) {
  std::array<Tetromino, WIDTH> emptyRow;
  emptyRow.fill(Tetromino::Empty);
  grid.fill(emptyRow);
}

void Playfield::restart() {
  *this = Playfield();
}

bool Playfield::isValidPosition(const FallingPiece& piece) const {
  return std::all_of(piece.tetrominoMap.begin(), piece.tetrominoMap.end(),
    [&](const CoordinatePair& coordinates) {
    int i = coordinates.x + piece.horizontalPosition;
    int j = coordinates.y + piece.verticalPosition;
    return i >= 0 && i < WIDTH && j >= 0 && j < HEIGHT && grid[j][i] == Tetromino::Empty;
  });
}

bool Playfield::tryShifting(Shift shift) {
  const FallingPiece shiftedPiece = fallingPiece.shifted(shift);
  if (isValidPosition(shiftedPiece)) {
    fallingPiece = shiftedPiece;
    lockDelayFrames = 0;
    lockDelayMoves += 1;

    return true;
  }

  return false;
}

static OffsetTable calculateOffsetDifferences(const OffsetTable& startOffsets, const OffsetTable& endOffsets) {
  OffsetTable result;
  std::transform(startOffsets.begin(), startOffsets.end(), endOffsets.begin(), result.begin(),
    [](const CoordinatePair& p1, const CoordinatePair& p2) -> CoordinatePair {
    return { static_cast<signed char>(p1.x - p2.x), static_cast<signed char>(p1.y - p2.y) };
  });
  return result;
}

bool Playfield::tryRotating(RotationType rotationType) {
  const OffsetTable startOffsetValues = getOffsetTable(fallingPiece);
  const FallingPiece rotatedPiece = fallingPiece.rotated(rotationType);
  const OffsetTable endOffsetValues = getOffsetTable(rotatedPiece);
  const OffsetTable trueOffsets = calculateOffsetDifferences(startOffsetValues, endOffsetValues);

  for (const CoordinatePair& offset : trueOffsets) {
    const FallingPiece translatedPiece = rotatedPiece.translated(offset);
    if (isValidPosition(translatedPiece)) {
      fallingPiece = translatedPiece;
      lockDelayFrames = 0;
      lockDelayMoves += 1;
      return true;
    }
  }

  return false;
}

SpinType Playfield::isSpin() const {
  if (!wasLastMoveRotation) return SpinType::No;
  TetrominoMap corners = { {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}} };
  std::array<CoordinatePair, 2> frontCorners, backCorners;
  switch (fallingPiece.orientation) {
  case Orientation::Up:
    frontCorners = { {corners[0], corners[1]} };
    backCorners = { {corners[2], corners[3]} };
    break;
  case Orientation::Right:
    frontCorners = { {corners[1], corners[2]} };
    backCorners = { {corners[3], corners[0]} };
    break;
  case Orientation::Down:
    frontCorners = { {corners[2], corners[3]} };
    backCorners = { {corners[0], corners[1]} };
    break;
  case Orientation::Left:
    frontCorners = { {corners[3], corners[0]} };
    backCorners = { {corners[1], corners[2]} };
    break;
  }

  int frontCornerCount = std::count_if(frontCorners.begin(), frontCorners.end(),
    [&](const CoordinatePair& coordinates) {
    int i = fallingPiece.horizontalPosition + coordinates.x;
    int j = fallingPiece.verticalPosition + coordinates.y;
    return i < 0 || i >= WIDTH || j < 0 || j >= HEIGHT || grid[j][i] != Tetromino::Empty;
  });

  int backCornerCount = std::count_if(backCorners.begin(), backCorners.end(),
    [&](const CoordinatePair& coordinates) {
    int i = fallingPiece.horizontalPosition + coordinates.x;
    int j = fallingPiece.verticalPosition + coordinates.y;
    return i < 0 || i >= WIDTH || j < 0 || j >= HEIGHT || grid[j][i] != Tetromino::Empty;
  });

  if (frontCornerCount + backCornerCount < 3) return SpinType::No;
  if (frontCornerCount == 2) return SpinType::Proper;
  return SpinType::Mini;
}

void Playfield::solidify() {
  bool anyMinoSolidifedAboveVisibleHeight = false;

  for (const CoordinatePair& pair : fallingPiece.tetrominoMap) {
    int i = pair.x + fallingPiece.horizontalPosition;
    int j = pair.y + fallingPiece.verticalPosition;
    grid[j][i] = fallingPiece.tetromino;

    if (j >= VISIBLE_HEIGHT)
      anyMinoSolidifedAboveVisibleHeight = true;
  }

  clearLines();
  replaceNextPiece();
  canSwap = true;

  bool canSpawnPiece = std::all_of(fallingPiece.tetrominoMap.begin(), fallingPiece.tetrominoMap.end(),
    [&](const CoordinatePair& coordinates) {
    int i = coordinates.x + fallingPiece.horizontalPosition;
    int j = coordinates.y + fallingPiece.verticalPosition;
    return grid[j][i] == Tetromino::Empty;
  });

  hasLost = !anyMinoSolidifedAboveVisibleHeight || !canSpawnPiece;
}

void Playfield::clearRows(const std::vector<std::size_t>& rowsToClear) {
  for (std::size_t rowIndex : rowsToClear) {
    std::copy_backward(grid.begin(), grid.begin() + rowIndex, grid.begin() + rowIndex + 1);
    grid[0].fill(Tetromino::Empty);
  }
}

bool Playfield::isAllClear() const {
  for (const auto& row : grid)
    for (const auto& mino : row)
      if (mino != Tetromino::Empty)
        return false;
  return true;
}

void Playfield::clearLines() {
  std::vector<std::size_t> rowsToClear;
  for (std::size_t j = 0; j < HEIGHT && rowsToClear.size() != 4; ++j) {
    bool isRowFull = std::all_of(grid[j].begin(), grid[j].end(),
      [](auto mino) {return mino != Tetromino::Empty;}
    );

    if (isRowFull) rowsToClear.push_back(j);
  }

  SpinType spinType = (fallingPiece.tetromino == Tetromino::T) ? isSpin() : SpinType::No;
  clearRows(rowsToClear);
  updateScore(rowsToClear.size(), spinType);
}

void Playfield::updateScore(std::size_t clearedLines, SpinType spinType) {
  if (clearedLines == 0) {
    combo = 0;
  } else {
    combo += 1;
    score += combo * 50;
    if ((clearedLines == 4) || (spinType != SpinType::No)) {
      b2b += 1;
    } else {
      b2b = 0;
    }
  }

  float b2bFactor = (b2b >= 2) ? 1.5f : 1.0f;

  if (spinType == SpinType::Proper) {
    if (clearedLines == 1) {
      message = MessageType::SINGLE;
      score += 800 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::DOUBLE;
      score += 1200 * b2bFactor;
    } else if (clearedLines == 3) {
      message = MessageType::TRIPLE;
      score += 1600 * b2bFactor;
    } else {
      message = MessageType::EMPTY;
      score += 400 * b2bFactor;
    }
    message.spinType = SpinType::Proper;
  } else if (spinType == SpinType::Mini) {
    if (clearedLines == 1) {
      message = MessageType::SINGLE;
      score += 200 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::DOUBLE;
      score += 400 * b2bFactor;
    } else {
      message = MessageType::EMPTY;
      score += 100 * b2bFactor;
    }
    message.spinType = SpinType::Mini;
  } else {
    if (clearedLines == 1) {
      message = MessageType::SINGLE;
      score += 100 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::DOUBLE;
      score += 300 * b2bFactor;
    } else if (clearedLines == 3) {
      message = MessageType::TRIPLE;
      score += 500 * b2bFactor;
    } else if (clearedLines == 4) {
      message = MessageType::TETRIS;
      score += 800 * b2bFactor;
    }
  }


  if (isAllClear()) {
    message = MessageType::ALLCLEAR;
    score += 3500 * b2bFactor;
  }
}

FallingPiece Playfield::getGhostPiece() const {
  FallingPiece ghostPiece = fallingPiece;
  while (isValidPosition(ghostPiece.fallen())) {
    ghostPiece.fall();
  }
  return ghostPiece;
}

void Playfield::swapTetromino() {
  Tetromino currentTetromino = fallingPiece.tetromino;
  fallingPiece = FallingPiece(holdingPiece, INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION);
  holdingPiece = currentTetromino;
  canSwap = false;
  framesSinceLastFall = 0;
  lockDelayFrames = 0;
  lockDelayMoves = 0;
}

void Playfield::replaceNextPiece() {
  Tetromino newTetromino = nextQueue.getNextTetromino();
  fallingPiece = FallingPiece(newTetromino, INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION);
  framesSinceLastFall = 0;
  lockDelayFrames = 0;
  lockDelayMoves = 0;
}

void Playfield::updateTimers() {
  framesSinceLastFall += 1;
  lockDelayFrames += 1;
  if (message.timer > 0)
    message.timer -= 1;
}

void Playfield::handleSpecialInput() {
  if (checkRestartInput()) restart();
  if (checkSwapInput() && canSwap) {
    swapTetromino();
    wasLastMoveRotation = false;
  }
}

void Playfield::handleShiftInput() {
  if (checkLeftInput()) {
    if (tryShifting(Shift::Left))
      wasLastMoveRotation = false;
  } else if (checkRightInput()) {
    if (tryShifting(Shift::Right))
      wasLastMoveRotation = false;
  }

  if (checkLeftDasInput()) {
    if (signedFramesPressed < 0)
      signedFramesPressed = 0;
    signedFramesPressed += 1;
    while (signedFramesPressed > DAS) {
      if (!tryShifting(Shift::Left))
        break;
      else
        wasLastMoveRotation = false;
    }
  } else if (checkRightDasInput()) {
    if (signedFramesPressed > 0)
      signedFramesPressed = 0;
    signedFramesPressed -= 1;
    while (-signedFramesPressed > DAS) {
      if (!tryShifting(Shift::Right))
        break;
      else
        wasLastMoveRotation = false;
    }
  } else {
    signedFramesPressed = 0;
  }
}

void Playfield::handleRotationInput() {
  if (checkClockwiseInput()) {
    if (tryRotating(RotationType::Clockwise))
      wasLastMoveRotation = true;
  } else if (checkCounterClockwiseInput()) {
    if (tryRotating(RotationType::CounterClockwise))
      wasLastMoveRotation = true;
  } else if (checkOneEightyInput()) {
    if (tryRotating(RotationType::OneEighty))
      wasLastMoveRotation = true;
  }
}

bool Playfield::handleDropInput() {
  if (checkHardDropInput()) {
    // Hard Drop
    if (isValidPosition(fallingPiece.fallen())) wasLastMoveRotation = false;
    fallingPiece = getGhostPiece();
    solidify();
    return true;
  }

  bool isFallStep = false;
  if (checkSoftDropInput()) {
    // Soft Drop
    if (framesSinceLastFall >= SOFT_DROP_FRAMES) {
      framesSinceLastFall = 0;
      isFallStep = true;
    }
  } else if (framesSinceLastFall >= GRAVITY_FRAMES) {
    // Gravity
    framesSinceLastFall = 0;
    isFallStep = true;
  }

  if (isValidPosition(fallingPiece.fallen())) {
    // Is not touching ground
    if (isFallStep) {
      wasLastMoveRotation = false;
      fallingPiece.fall();
      lockDelayFrames = 0;
      lockDelayMoves = 0;
    }

    return false;
  }
  // Is touching ground
  if (lockDelayFrames > MAX_LOCK_DELAY_FRAMES || lockDelayMoves > MAX_LOCK_DELAY_RESETS) {
    solidify();
    return true;
  }
  return false;
}

bool Playfield::update() {
  if (hasLost) return false;
  handleSpecialInput();
  updateTimers();
  nextQueue.pushNewBagIfNeeded();
  if (fallingPiece.tetromino == Tetromino::Empty) replaceNextPiece();
  handleShiftInput();
  handleRotationInput();
  return handleDropInput();
}