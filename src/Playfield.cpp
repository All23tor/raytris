#include "Playfield.hpp"
#include <algorithm>

Playfield::Playfield() : fallingPiece(Tetromino::EMPTY, INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION) {
  std::array<Tetromino, WIDTH> emptyRow;
  emptyRow.fill(Tetromino::EMPTY);
  grid.fill(emptyRow);
}

void Playfield::restart() {
  *this = Playfield();
}

bool Playfield::checkFallingCollisions() {
  return std::all_of(fallingPiece.tetrominoMap.begin(), fallingPiece.tetrominoMap.end(),
    [&](const CoordinatePair& pair) {
    int i = pair.x + fallingPiece.horizontalPosition;
    int j = pair.y + fallingPiece.verticalPosition;
    return i <= HEIGHT - 1 && grid[j][i] == Tetromino::EMPTY;
  });
}

bool Playfield::shiftFallingPiece(Shift shift) {
  FallingPiece oldPiece = fallingPiece;
  switch (shift) {
  case Shift::LEFT:
    fallingPiece.shiftLeft();
    break;
  case Shift::RIGHT:
    fallingPiece.shiftRight();
    break;
  }

  bool passedCheck = std::all_of(fallingPiece.tetrominoMap.begin(), fallingPiece.tetrominoMap.end(),
    [&](const CoordinatePair& pair) {
    int i = pair.x + fallingPiece.horizontalPosition;
    int j = pair.y + fallingPiece.verticalPosition;
    return i >= 0 && i < WIDTH && grid[j][i] == Tetromino::EMPTY;
  });

  if (!passedCheck) {
    fallingPiece = oldPiece;
  } else {
    lockDelayFrames = 0;
    lockDelayMoves += 1;
  }

  return passedCheck;
}

void Playfield::checkRotationCollision(RotationType rotationType) {
  FallingPiece oldPiece = fallingPiece;
  OffsetTable startOffsetValues = getOffsetTable(fallingPiece);
  switch (rotationType) {
  case RotationType::Clockwise:
    fallingPiece.turnClockwise();
    break;
  case RotationType::CounterClockwise:
    fallingPiece.turnCounterClockwise();
    break;
  case RotationType::OneEighty:
    fallingPiece.turnClockwise();
    fallingPiece.turnClockwise();
    break;
  }
  OffsetTable endOffsetValues = getOffsetTable(fallingPiece);
  OffsetTable offsets;
  std::transform(startOffsetValues.begin(), startOffsetValues.end(), endOffsetValues.begin(), offsets.begin(),
    [](const CoordinatePair& p1, const CoordinatePair& p2) -> CoordinatePair {
    return { static_cast<signed char>(p1.x - p2.x), static_cast<signed char>(p1.y - p2.y) };
  });

  bool couldRotate = false;
  for (const CoordinatePair& offset : offsets) {
    int new_horizontal_position = fallingPiece.horizontalPosition + offset.x;
    int new_vertical_position = fallingPiece.verticalPosition - offset.y;

    bool passed = std::all_of(fallingPiece.tetrominoMap.begin(), fallingPiece.tetrominoMap.end(),
      [&](const CoordinatePair& coordinates) {
      int i = coordinates.x + new_horizontal_position;
      int j = coordinates.y + new_vertical_position;
      return  i >= 0 && i < WIDTH && j < HEIGHT && grid[j][i] == Tetromino::EMPTY;
    });

    if (passed) {
      fallingPiece.horizontalPosition = new_horizontal_position;
      fallingPiece.verticalPosition = new_vertical_position;
      couldRotate = true;
      break;
    }
  }

  if (!couldRotate) {
    fallingPiece = oldPiece;
  } else {
    lockDelayFrames = 0;
    lockDelayMoves += 1;
  }
}

void Playfield::solidifyFallingPiece() {
  bool passed = false;
  for (const CoordinatePair& pair : fallingPiece.tetrominoMap) {
    int i = pair.x + fallingPiece.horizontalPosition;
    int j = pair.y + fallingPiece.verticalPosition;
    grid[j][i] = fallingPiece.tetromino;

    if (j >= VISIBLE_HEIGHT)
      passed = true;
  }

  replaceNextPiece();
  canSwap = true;

  for (const CoordinatePair& coordinates : fallingPiece.tetrominoMap) {
    int i = coordinates.x + fallingPiece.horizontalPosition;
    int j = coordinates.y + fallingPiece.verticalPosition;
    if (grid[j][i] != Tetromino::EMPTY)
      passed = false;
    break;
  }

  hasLost = !passed;
  std::size_t linesCleared = clearLines();
  updateScore(linesCleared);
}

void Playfield::clearRows(std::vector<std::size_t>& rowsToClear) {
  for (int count = 0; !rowsToClear.empty(); rowsToClear.pop_back()) {
    for (int row = rowsToClear.back() + count++; row > 0; --row) {
      grid[row] = std::move(grid[row - 1]);
    }
    grid[0].fill(Tetromino::EMPTY);
  }
}

bool Playfield::isAllClear() const {
  for (const auto& row : grid)
    for (const auto& mino : row)
      if (mino != Tetromino::EMPTY)
        return false;
  return true;
}

std::size_t Playfield::clearLines() {
  std::vector<std::size_t> rowsToClear;
  for (std::size_t j = 0; j < HEIGHT && rowsToClear.size() != 4; ++j) {
    bool isRowFull = std::all_of(grid[j].begin(), grid[j].end(),
      [](auto mino) {return mino != Tetromino::EMPTY;}
    );

    if (isRowFull) rowsToClear.push_back(j);
  }

  std::size_t clearedLines = rowsToClear.size();
  clearRows(rowsToClear);

  return clearedLines;
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
  FallingPiece oldGhostPiece = fallingPiece;
  while (true) {
    ghostPiece.fall();
    bool passed = true;
    for (const CoordinatePair& pair : ghostPiece.tetrominoMap) {
      int i = pair.x + ghostPiece.horizontalPosition;
      int j = pair.y + ghostPiece.verticalPosition;
      if (j > HEIGHT - 1 || grid[j][i] != Tetromino::EMPTY) {
        passed = false;
        break;
      }
    }
    if (!passed) {
      ghostPiece = oldGhostPiece;
      break;
    }
    oldGhostPiece = ghostPiece;
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

  if (fallingPiece.tetromino == Tetromino::EMPTY) {
    replaceNextPiece();
  }

  // Shift
  if (IsKeyPressed(KEY_LEFT)) {
    shiftFallingPiece(Shift::LEFT);
  } else if (IsKeyPressed(KEY_RIGHT)) {
    shiftFallingPiece(Shift::RIGHT);
  }

  // DAS
  if (IsKeyDown(KEY_LEFT)) {
    if (signedFramesPressed < 0)
      signedFramesPressed = 0;
    signedFramesPressed += 1;
    while (signedFramesPressed > DAS) {
      if (!shiftFallingPiece(Shift::LEFT))
        break;
    }
  } else if (IsKeyDown(KEY_RIGHT)) {
    if (signedFramesPressed > 0)
      signedFramesPressed = 0;
    signedFramesPressed -= 1;
    while (-signedFramesPressed > DAS) {
      if (!shiftFallingPiece(Shift::RIGHT))
        break;
    }
  } else {
    signedFramesPressed = 0;
  }

  // Rotations
  if (IsKeyPressed(KEY_UP)) {
    checkRotationCollision(RotationType::Clockwise);
  } else if (IsKeyPressed(KEY_Z)) {
    checkRotationCollision(RotationType::CounterClockwise);
  } else if (IsKeyPressed(KEY_A)) {
    checkRotationCollision(RotationType::OneEighty);
  }

  if (IsKeyPressed(KEY_SPACE)) {
    // Hard Drop
    fallingPiece = getGhostPiece();
    solidifyFallingPiece();
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

  FallingPiece oldPiece = fallingPiece;
  fallingPiece.fall();
  if (checkFallingCollisions()) {
    // Is not touching ground
    if (isFallStep) {
      lockDelayFrames = 0;
      lockDelayMoves = 0;
    } else {
      fallingPiece = oldPiece;
    }

    return false;
  }
  // Is touching ground
  fallingPiece = oldPiece;

  if (lockDelayFrames > MAX_LOCK_DELAY_FRAMES || lockDelayMoves > MAX_LOCK_DELAY_RESETS) {
    solidifyFallingPiece();
    return true;
  }
  return false;
}
