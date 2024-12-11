#include "Playfield.hpp"
#include <algorithm>

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

void Playfield::solidify() {
  bool anyMinoSolidifedAboveVisibleHeight = false;

  for (const CoordinatePair& pair : fallingPiece.tetrominoMap) {
    int i = pair.x + fallingPiece.horizontalPosition;
    int j = pair.y + fallingPiece.verticalPosition;
    grid[j][i] = fallingPiece.tetromino;

    if (j >= VISIBLE_HEIGHT)
      anyMinoSolidifedAboveVisibleHeight = true;
  }

  replaceNextPiece();
  canSwap = true;

  bool canSpawnPiece = std::all_of(fallingPiece.tetrominoMap.begin(), fallingPiece.tetrominoMap.end(),
    [&](const CoordinatePair& coordinates) {
    int i = coordinates.x + fallingPiece.horizontalPosition;
    int j = coordinates.y + fallingPiece.verticalPosition;
    return grid[j][i] == Tetromino::Empty;
  });

  hasLost = !anyMinoSolidifedAboveVisibleHeight || !canSpawnPiece;
  clearLines();
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

  clearRows(rowsToClear);
  updateScore(rowsToClear.size());
}

void Playfield::updateScore(std::size_t clearedLines) {
  if (clearedLines == 0) {
    combo = 0;
    return;
  }

  if (clearedLines == 4) {
    b2b += 1;
  } else {
    b2b = 0;
  }

  combo += 1;
  score += combo * 50;

  float b2bFactor = b2b >= 2 ? 1.5f : 1.0f;

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

bool Playfield::update() {
  if (IsKeyPressed(KEY_R))
    restart();
  if (hasLost)
    return false;

  if (IsKeyPressed(KEY_C) && canSwap) {
    swapTetromino();
  }

  updateTimers();
  nextQueue.pushNewBagIfNeeded();

  if (fallingPiece.tetromino == Tetromino::Empty) {
    replaceNextPiece();
  }

  // Shift
  if (IsKeyPressed(KEY_LEFT)) {
    tryShifting(Shift::Left);
  } else if (IsKeyPressed(KEY_RIGHT)) {
    tryShifting(Shift::Right);
  }

  // DAS
  if (IsKeyDown(KEY_LEFT)) {
    if (signedFramesPressed < 0)
      signedFramesPressed = 0;
    signedFramesPressed += 1;
    while (signedFramesPressed > DAS) {
      if (!tryShifting(Shift::Left))
        break;
    }
  } else if (IsKeyDown(KEY_RIGHT)) {
    if (signedFramesPressed > 0)
      signedFramesPressed = 0;
    signedFramesPressed -= 1;
    while (-signedFramesPressed > DAS) {
      if (!tryShifting(Shift::Right))
        break;
    }
  } else {
    signedFramesPressed = 0;
  }

  // Rotations
  if (IsKeyPressed(KEY_UP)) {
    tryRotating(RotationType::Clockwise);
  } else if (IsKeyPressed(KEY_Z)) {
    tryRotating(RotationType::CounterClockwise);
  } else if (IsKeyPressed(KEY_A)) {
    tryRotating(RotationType::OneEighty);
  }

  if (IsKeyPressed(KEY_SPACE)) {
    // Hard Drop
    fallingPiece = getGhostPiece();
    solidify();
    return true;
  }

  bool isFallStep = false;
  if (IsKeyDown(KEY_DOWN)) {
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
