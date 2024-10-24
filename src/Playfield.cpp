#include "Playfield.hpp"

Playfield::Playfield()
    : fallingPiece(Tetromino::EMPTY, INITIAL_HORIAZONTAL_POSITION,
                   INITIAL_VERTICAL_POSITION) {
  std::array<Tetromino, WIDTH> emptyRow;
  emptyRow.fill(Tetromino::EMPTY);
  grid.fill((emptyRow));
}

void Playfield::restart() { *this = Playfield(); }

bool Playfield::checkFallingCollisions() {
  for (const CoordinatePair &pair : fallingPiece.tetrominoMap) {
    int i = pair.x + fallingPiece.horizontalPosition;
    int j = pair.y + fallingPiece.verticalPosition;
    if (j > HEIGHT - 1 || grid[j][i] != Tetromino::EMPTY) {
      return false;
    }
  }
  return true;
}

bool Playfield::shiftFallingPiece(Shift shift) {
  FallingPiece old_piece = fallingPiece;
  switch (shift) {
  case Shift::LEFT:
    fallingPiece.shiftLeft();
    break;
  case Shift::RIGHT:
    fallingPiece.shiftRight();
    break;
  }
  bool passedCheck = true;

  for (const CoordinatePair &pair : fallingPiece.tetrominoMap) {
    int i = pair.x + fallingPiece.horizontalPosition;
    int j = pair.y + fallingPiece.verticalPosition;
    if (i < 0 || i >= WIDTH || grid[j][i] != Tetromino::EMPTY) {
      passedCheck = false;
      break;
    }
  }

  if (!passedCheck) {
    fallingPiece = old_piece;
  } else {
    lockDelayFrames = 0;
    lockDelayMoves += 1;
  }

  return passedCheck;
}

void Playfield::checkRotationCollision(RotationType rotationType) {
  bool couldRotate = false;
  FallingPiece old_piece = fallingPiece;
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

  for (int offsetNumber = 0; offsetNumber < startOffsetValues.size();
       ++offsetNumber) {
    bool passed = true;
    int new_horizontal_position = fallingPiece.horizontalPosition +
                                  startOffsetValues[offsetNumber].x -
                                  endOffsetValues[offsetNumber].x;
    int new_vertical_position = fallingPiece.verticalPosition -
                                startOffsetValues[offsetNumber].y +
                                endOffsetValues[offsetNumber].y;

    TetrominoMap map = fallingPiece.tetrominoMap;
    for (const CoordinatePair &coordinates : map) {
      int i = coordinates.x + new_horizontal_position;
      int j = coordinates.y + new_vertical_position;
      if (grid[j][i] != Tetromino::EMPTY || i < 0 || i >= WIDTH ||
          j >= HEIGHT) {
        passed = false;
        break;
      }
    }

    if (passed) {
      fallingPiece.horizontalPosition = new_horizontal_position;
      fallingPiece.verticalPosition = new_vertical_position;
      couldRotate = true;
      break;
    }
  }

  if (!couldRotate) {
    fallingPiece = old_piece;
  } else {
    lockDelayFrames = 0;
    lockDelayMoves += 1;
  }
}

void Playfield::solidifyFallingPiece() {
  bool passed = false;
  for (const CoordinatePair &pair : fallingPiece.tetrominoMap) {
    int i = pair.x + fallingPiece.horizontalPosition;
    int j = pair.y + fallingPiece.verticalPosition;
    grid[j][i] = fallingPiece.tetromino;

    if (j >= VISIBLE_HEIGHT)
      passed = true;
  }

  Tetromino new_tetromino = nextQueue.getNextTetromino();
  fallingPiece = FallingPiece(new_tetromino, INITIAL_HORIAZONTAL_POSITION,
                              INITIAL_VERTICAL_POSITION);
  canSwap = true;

  TetrominoMap map = fallingPiece.tetrominoMap;
  for (const CoordinatePair &coordinates : map) {
    int i = coordinates.x + fallingPiece.horizontalPosition;
    int j = coordinates.y + fallingPiece.verticalPosition;
    if (grid[j][i] != Tetromino::EMPTY)
      passed = false;
    break;
  }

  hasLost = !passed;

  framesSinceLastFall = 0;
  lockDelayFrames = 0;
  lockDelayMoves = 0;
}

void Playfield::clearRows(std::vector<int> &rowIDs, char count = 0) {
  if (rowIDs.empty())
    return;

  int rows_to_clear = rowIDs.back() + count;
  for (int row = rows_to_clear; row > 0; --row) {
    for (int i = 0; i < WIDTH; ++i) {
      grid[row][i] = grid[row - 1][i];
    }
  }

  for (auto &mino : grid[0]) {
    mino = Tetromino::EMPTY;
  }

  rowIDs.pop_back();
  clearRows(rowIDs, count + 1);
}

bool Playfield::isAllClear() {
  for (const auto &row : grid)
    for (const auto &mino : row)
      if (mino != Tetromino::EMPTY)
        return false;
  return true;
}

void Playfield::clearLines() {
  std::vector<int> rowsToClear;
  for (int j = 0; j < HEIGHT; ++j) {
    bool allTrue = true;
    for (const auto &mino : grid[j]) {
      if (mino == Tetromino::EMPTY) {
        allTrue = false;
        break;
      }
    }

    if (allTrue) {
      rowsToClear.push_back(j);
      if (rowsToClear.size() >= 4)
        break;
    }
  }

  std::size_t size = rowsToClear.size();

  if (size == 0) {
    combo = 0;
    return;
  }

  if (size != 4) {
    b2b = 0;
  } else {
    b2b += 1;
  }

  combo += 1;
  score += combo * 50;

  float b2bFactor = b2b >= 2 ? 1.5f : 1.0f;

  if (size == 1) {
    message = MessageType::SINGLE;
    score += 100 * b2bFactor;
  } else if (size == 2) {
    message = MessageType::DOUBLE;
    score += 300 * b2bFactor;
  } else if (size == 3) {
    message = MessageType::TRIPLE;
    score += 500 * b2bFactor;
  } else if (size == 4) {
    message = MessageType::TETRIS;
    score += 800 * b2bFactor;
  }

  clearRows(rowsToClear);

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
    for (const CoordinatePair &pair : ghostPiece.tetrominoMap) {
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
  fallingPiece = FallingPiece(holdingPiece, INITIAL_HORIAZONTAL_POSITION,
                              INITIAL_VERTICAL_POSITION);
  holdingPiece = currentTetromino;
  canSwap = false;
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
  bool solidified = false;
  if (IsKeyPressed(KEY_R))
    restart();
  if (hasLost)
    return solidified;

  if (IsKeyPressed(KEY_C) && canSwap) {
    swapTetromino();
  }

  updateTimers();
  nextQueue.pushNewBagIfNeeded();

  if (fallingPiece.tetromino == Tetromino::EMPTY) {
    Tetromino new_tetromino = nextQueue.getNextTetromino();
    fallingPiece = FallingPiece(new_tetromino, INITIAL_HORIAZONTAL_POSITION,
                                INITIAL_VERTICAL_POSITION);
    framesSinceLastFall = 0;
    lockDelayFrames = 0;
    lockDelayMoves = 0;
  }

  if (IsKeyPressed(KEY_LEFT)) {
    shiftFallingPiece(Shift::LEFT);
  } else if (IsKeyPressed(KEY_RIGHT)) {
    shiftFallingPiece(Shift::RIGHT);
  }

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

  if (IsKeyPressed(KEY_UP)) {
    checkRotationCollision(RotationType::Clockwise);
  } else if (IsKeyPressed(KEY_Z)) {
    checkRotationCollision(RotationType::CounterClockwise);
  } else if (IsKeyPressed(KEY_A)) {
    checkRotationCollision(RotationType::OneEighty);
  }

  bool isFallStep = false;

  if (IsKeyPressed(KEY_SPACE)) {
    FallingPiece old_piece = fallingPiece;
    while (true) {
      fallingPiece.fall();
      if (!checkFallingCollisions()) {
        fallingPiece = old_piece;
        solidifyFallingPiece();
        solidified = true;
        break;
      }
      old_piece = fallingPiece;
      score += 2;
    }
    lockDelayMoves = 0;
    lockDelayFrames = 0;
    clearLines();
  } else {
    FallingPiece old_piece = fallingPiece;
    fallingPiece.fall();

    if (IsKeyDown(KEY_DOWN)) {
      if (framesSinceLastFall >= SOFT_DROP_FRAMES) {
        framesSinceLastFall = 0;
        isFallStep = true;
      }
    } else if (framesSinceLastFall >= GRAVITY_FRAMES) {
      framesSinceLastFall = 0;
      isFallStep = true;
    }

    if (!checkFallingCollisions()) {
      fallingPiece = old_piece;

      if (lockDelayFrames > MAX_LOCK_DELAY_FRAMES ||
          lockDelayMoves > MAX_LOCK_DELAY_RESETS) {
        solidifyFallingPiece();
        solidified = true;
        clearLines();
      }
    } else {
      if (isFallStep) {
        lockDelayFrames = 0;
        lockDelayMoves = 0;
      } else {
        fallingPiece = old_piece;
      }
    }
  }

  return solidified;
}
