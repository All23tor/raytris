#include "Playfield.hpp"
#include "DrawingDetails.hpp"
#include "FallingPiece.hpp"
#include "HandlingSettings.hpp"
#include "raylib.h"
#include <format>
#include <cmath>
#include <algorithm>

Playfield::Playfield() : fallingPiece(Tetromino::Empty, InitialHorizontalPosition, InitialVerticalPosition) {
  std::array<Tetromino, Width> emptyRow;
  emptyRow.fill(Tetromino::Empty);
  grid.fill(emptyRow);
}

void Playfield::restart() {
  auto lastScore = this->score;
  *this = Playfield();
  this->score = lastScore;
}

namespace {
  std::ostream& operator<<(std::ostream& out, const std::array<std::array<Tetromino, Playfield::Width>, Playfield::Height>& grid) {
    for (const auto& row: grid) {
      for (const auto& mino: row) {
        out << static_cast<int>(mino) << ' ';
      }
      out << '\n';
    }
    return out;
  }

  std::istream& operator>>(std::istream& in, std::array<std::array<Tetromino, Playfield::Width>, Playfield::Height>& grid) {
    for (auto& row: grid) {
      for (auto& mino: row) {
        int input_mino;
        in >> input_mino;
        mino = static_cast<Tetromino>(input_mino);
      }
    }
    return in;
  }

  std::ostream& operator<<(std::ostream& out, const FallingPiece& fp) {
    out << static_cast<int>(fp.tetromino) << ' ' << static_cast<int>(fp.orientation) << '\n';
    out << static_cast<int>(fp.horizontalPosition) << ' ' << static_cast<int>(fp.verticalPosition) << '\n';
    for (const auto& coord: fp.tetrominoMap) {
      out << static_cast<int>(coord.x) << ' ' << static_cast<int>(coord.y) << '\n';
    }
    return out;
  }

  std::istream& operator>>(std::istream& in, FallingPiece& fp) {
    int input_tetromino, input_orientation, input_horizontal, input_vertical;
    in >> input_tetromino >> input_orientation;
    in >> input_horizontal >> input_vertical;
    fp.tetromino = static_cast<Tetromino>(input_tetromino);
    fp.orientation = static_cast<Orientation>(input_orientation);
    fp.horizontalPosition = input_horizontal;
    fp.verticalPosition = input_vertical;

    for (auto& coord: fp.tetrominoMap) {
      int input_coord_x, input_coord_y;
      in >> input_coord_x >> input_coord_y;
      coord = {static_cast<char>(input_coord_x), static_cast<char>(input_coord_y)}; 
    }
    return in;
  }

  std::ostream& operator<<(std::ostream& out, const Tetromino& tetromino) {
    return out << static_cast<int>(tetromino) << '\n'; 
  }

  std::istream& operator>>(std::istream& in, Tetromino& tetromino) {
    int input_tetromino;
    in >> input_tetromino;
    tetromino = static_cast<Tetromino>(input_tetromino);
    return in;
  }

  std::ostream& operator<<(std::ostream& out, const LineClearMessage& message) {
    return out << static_cast<int>(message.message) << static_cast<int>(message.timer) << static_cast<int>(message.spinType) << '\n';
  }
  
  std::istream& operator>>(std::istream& in, LineClearMessage& message) {
    int input_message, input_timer, input_spin_type;
    in >> input_message >> input_timer >> input_spin_type;
    message.message = static_cast<MessageType>(input_message);
    message.timer = input_timer;
    message.spinType = static_cast<SpinType>(input_spin_type);
    return in;  
  }
}

std::ostream& operator<<(std::ostream& out, const Playfield& p) {
  out << p.grid << '\n';
  out << p.fallingPiece << '\n';
  out << p.holdingPiece << '\n';
  out << p.nextQueue << '\n'; 
  out << p.canSwap << '\n';
  out << p.framesSinceLastFall << '\n';
  out << p.lockDelayFrames << '\n';
  out << p.lockDelayMoves << '\n';
  out << p.signedFramesPressed << '\n';
  out << p.combo << '\n';
  out << p.hasLost << '\n';
  out << p.score << '\n';
  out << p.b2b << '\n';
  out << p.wasLastMoveRotation << '\n';
  out << p.message << '\n';
  return out;
}

std::istream& operator>>(std::istream& in, Playfield& p) {
  in >> p.grid;
  in >> p.fallingPiece;
  in >> p.holdingPiece;
  in >> p.nextQueue;
  in >> p.canSwap;
  in >> p.framesSinceLastFall;
  in >> p.lockDelayFrames;
  in >> p.lockDelayMoves;
  in >> p.signedFramesPressed;
  in >> p.combo;
  in >> p.hasLost;
  in >> p.score;
  in >> p.b2b;
  in >> p.wasLastMoveRotation;
  in >> p.message;
  return in;
}

bool Playfield::lost() const {
  return hasLost;
}

bool Playfield::isValidPosition(const FallingPiece& piece) const {
  return std::all_of(piece.tetrominoMap.begin(), piece.tetrominoMap.end(),
    [&](const CoordinatePair& coordinates) {
    int i = coordinates.x + piece.horizontalPosition;
    int j = coordinates.y + piece.verticalPosition;
    return i >= 0 && i < Width && j >= 0 && j < Height && grid[j][i] == Tetromino::Empty;
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

namespace {
  OffsetTable calculateOffsetDifferences(const OffsetTable& startOffsets, const OffsetTable& endOffsets) {
    OffsetTable result;
    std::transform(startOffsets.begin(), startOffsets.end(), endOffsets.begin(), result.begin(),
      [](const CoordinatePair& p1, const CoordinatePair& p2) -> CoordinatePair {
      return { static_cast<signed char>(p1.x - p2.x), static_cast<signed char>(p1.y - p2.y) };
    });
    return result;
  }
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
    return i < 0 || i >= Width || j < 0 || j >= Height || grid[j][i] != Tetromino::Empty;
  });

  int backCornerCount = std::count_if(backCorners.begin(), backCorners.end(),
    [&](const CoordinatePair& coordinates) {
    int i = fallingPiece.horizontalPosition + coordinates.x;
    int j = fallingPiece.verticalPosition + coordinates.y;
    return i < 0 || i >= Width || j < 0 || j >= Height || grid[j][i] != Tetromino::Empty;
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

    if (j >= VisibleHeight)
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

bool Playfield::isAllClear() const {
  for (const auto& row : grid)
    for (const auto& mino : row)
      if (mino != Tetromino::Empty)
        return false;
  return true;
}

void Playfield::clearLines() {
  std::array<std::size_t, 4> rowsToClear;
  std::size_t numberRowsToClear = 0;
  for (std::size_t j = 0; j < Height && numberRowsToClear != 4; ++j) {
    bool isRowFull = std::all_of(grid[j].begin(), grid[j].end(),
      [](auto mino) {return mino != Tetromino::Empty;}
    );

    if (isRowFull) {
      rowsToClear[numberRowsToClear] = j;
      numberRowsToClear++;
    }
  }

  SpinType spinType = (fallingPiece.tetromino == Tetromino::T) ? isSpin() : SpinType::No;
  for (std::size_t index = 0; index < numberRowsToClear; index++) {
    std::size_t rowIndex = rowsToClear[index];
    std::copy_backward(grid.begin(), grid.begin() + rowIndex, grid.begin() + rowIndex + 1);
    grid[0].fill(Tetromino::Empty);
  }
  updateScore(numberRowsToClear, spinType);
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
      message = MessageType::Single;
      score += 800 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::Double;
      score += 1200 * b2bFactor;
    } else if (clearedLines == 3) {
      message = MessageType::Triple;
      score += 1600 * b2bFactor;
    } else {
      message = MessageType::Empty;
      score += 400 * b2bFactor;
    }
    message.spinType = SpinType::Proper;
  } else if (spinType == SpinType::Mini) {
    if (clearedLines == 1) {
      message = MessageType::Single;
      score += 200 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::Double;
      score += 400 * b2bFactor;
    } else {
      message = MessageType::Empty;
      score += 100 * b2bFactor;
    }
    message.spinType = SpinType::Mini;
  } else {
    if (clearedLines == 1) {
      message = MessageType::Single;
      score += 100 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::Double;
      score += 300 * b2bFactor;
    } else if (clearedLines == 3) {
      message = MessageType::Triple;
      score += 500 * b2bFactor;
    } else if (clearedLines == 4) {
      message = MessageType::Tetris;
      score += 800 * b2bFactor;
    }
  }


  if (isAllClear()) {
    message = MessageType::AllClear;
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
  fallingPiece = FallingPiece(holdingPiece, InitialHorizontalPosition, InitialVerticalPosition);
  holdingPiece = currentTetromino;
  canSwap = false;
  framesSinceLastFall = 0;
  lockDelayFrames = 0;
  lockDelayMoves = 0;
}

void Playfield::replaceNextPiece() {
  Tetromino newTetromino = nextQueue.getNextTetromino();
  fallingPiece = FallingPiece(newTetromino, InitialHorizontalPosition, InitialVerticalPosition);
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

void Playfield::handleSpecialInput(const Controller& controller) {
  if (controller.checkSwapInput() && canSwap) {
    swapTetromino();
    wasLastMoveRotation = false;
  }
}

void Playfield::handleShiftInput(const Controller& controller, const HandlingSettings& settings) {
  if (controller.checkLeftInput()) {
    if (tryShifting(Shift::Left))
      wasLastMoveRotation = false;
  } else if (controller.checkRightInput()) {
    if (tryShifting(Shift::Right))
      wasLastMoveRotation = false;
  }

  if (controller.checkLeftDasInput()) {
    if (signedFramesPressed < 0)
      signedFramesPressed = 0;
    signedFramesPressed += 1;
    while (signedFramesPressed > settings.das) {
      if (!tryShifting(Shift::Left))
        break;
      else
        wasLastMoveRotation = false;
    }
  } else if (controller.checkRightDasInput()) {
    if (signedFramesPressed > 0)
      signedFramesPressed = 0;
    signedFramesPressed -= 1;
    while (-signedFramesPressed > settings.das) {
      if (!tryShifting(Shift::Right))
        break;
      else
        wasLastMoveRotation = false;
    }
  } else {
    signedFramesPressed = 0;
  }
}

void Playfield::handleRotationInput(const Controller& controller) {
  if (controller.checkClockwiseInput()) {
    if (tryRotating(RotationType::Clockwise))
      wasLastMoveRotation = true;
  } else if (controller.checkCounterClockwiseInput()) {
    if (tryRotating(RotationType::CounterClockwise))
      wasLastMoveRotation = true;
  } else if (controller.checkOneEightyInput()) {
    if (tryRotating(RotationType::OneEighty))
      wasLastMoveRotation = true;
  }
}

bool Playfield::handleDropInput(const Controller& controller, const HandlingSettings& settings) {
  if (controller.checkHardDropInput()) {
    // Hard Drop
    if (isValidPosition(fallingPiece.fallen())) wasLastMoveRotation = false;
    fallingPiece = getGhostPiece();
    solidify();
    return true;
  }

  bool isFallStep = false;
  if (controller.checkSoftDropInput()) {
    // Soft Drop
    if (framesSinceLastFall >= settings.softDropFrames) {
      framesSinceLastFall = 0;
      isFallStep = true;
    }
  } else if (framesSinceLastFall >= settings.gravityFrames) {
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
  if (lockDelayFrames > settings.maxLockDelayFrames || lockDelayMoves > settings.maxLockDelayResets) {
    solidify();
    return true;
  }
  return false;
}

bool Playfield::update(const Controller& controller, const HandlingSettings& settings) {
  if (hasLost) return false;
  handleSpecialInput(controller);
  updateTimers();
  nextQueue.pushNewBagIfNeeded();
  if (fallingPiece.tetromino == Tetromino::Empty) replaceNextPiece();
  handleShiftInput(controller, settings);
  handleRotationInput(controller);
  return handleDropInput(controller, settings);
}

namespace {
  constexpr Color getTetrominoColor(Tetromino tetromino) {
    switch (tetromino) {
    case Tetromino::I:
      return (Color) { 49, 199, 239, 255 };
    case Tetromino::O:
      return (Color) { 247, 211, 8, 255 };
    case Tetromino::T:
      return (Color) { 173, 77, 156, 255 };
    case Tetromino::S:
      return (Color) { 66, 182, 66, 255 };
    case Tetromino::Z:
      return (Color) { 239, 32, 41, 255 };
    case Tetromino::J:
      return (Color) { 90, 101, 173, 255 };
    case Tetromino::L:
      return (Color) { 239, 121, 33, 255 };
    default:
      return BLANK;
    }
  }

  inline Rectangle getBlockRectangle(int i, int j, const DrawingDetails& drawingDetails) {
    return { drawingDetails.position.x + i * drawingDetails.blockLength,
            drawingDetails.position.y + (j - static_cast<int>(Playfield::VisibleHeight)) * drawingDetails.blockLength,
            drawingDetails.blockLength, drawingDetails.blockLength };
  }

  void drawBlockPretty(int i, int j, const DrawingDetails& details, Color fill) {
    if (fill.a == 0)
      return;

    Rectangle rec = getBlockRectangle(i, j, details);
    DrawRectangleRec(rec, fill);
    DrawRectangle(rec.x + details.blockLength / 3, rec.y + details.blockLength / 3, rec.width / 3,
      rec.height / 3, DrawingDetails::DefaultPrettyOutline);
    DrawRectangleLinesEx(rec, details.blockLength / 8, DrawingDetails::DefaultPrettyOutline);
  }

  void drawBlockDanger(int i, int j, const DrawingDetails& details) { 
    Rectangle rec = getBlockRectangle(i, j, details);
    DrawRectangleLinesEx(rec, details.blockLength / 8, RED);
  DrawLineEx({rec.x + rec.width * 0.25f, rec.y + rec.height * 0.25f}, {rec.x + rec.width * 0.75f, rec.y + rec.height * 0.75f}, details.blockLength * 0.1f, RED);
    DrawLineEx({rec.x + rec.width * 0.75f, rec.y + rec.height * 0.25f}, {rec.x + rec.width * 0.25f, rec.y + rec.height * 0.75f}, details.blockLength * 0.1f, RED);
  }

  void drawPiece(const TetrominoMap& map, Color color, int horizontalOffset, int verticalOffset, const DrawingDetails& drawingDetails) {
    for (const CoordinatePair& coordinates : map) {
      int i = coordinates.x + horizontalOffset;
      int j = coordinates.y + verticalOffset;
      drawBlockPretty(i, j, drawingDetails, color);
    }
  }

  void drawPieceDanger(const FallingPiece& piece, const DrawingDetails& details) {
    for (const CoordinatePair& coordinates : piece.tetrominoMap) {
      int i = coordinates.x + piece.horizontalPosition;
      int j = coordinates.y + piece.verticalPosition;
      drawBlockDanger(i, j, details);
    }
  }
};

void Playfield::DrawTetrion(const DrawingDetails& drawingDetails) const {
  Rectangle tetrion = Rectangle{
      drawingDetails.position.x, drawingDetails.position.y,
      drawingDetails.blockLength * Width, drawingDetails.blockLength * VisibleHeight };
  DrawRectangleRec(tetrion, drawingDetails.TetrionBackgroundColor);
  DrawRectangleLinesEx(tetrion, drawingDetails.blockLength / 10, drawingDetails.GridlineColor);

  for (int i = 1; i < Width; ++i) {
    Rectangle rec = getBlockRectangle(i, VisibleHeight, drawingDetails);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx({ rec.x, rec.y },
      { rec.x, std::floor(rec.y + VisibleHeight * drawingDetails.blockLength) },
      drawingDetails.blockLength / 10, drawingDetails.GridlineColor);
  }

  for (int j = 1; j < VisibleHeight; ++j) {
    Rectangle rec = getBlockRectangle(0, j + VisibleHeight, drawingDetails);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx({ rec.x, rec.y },
      { std::floor(rec.x + drawingDetails.blockLength * Width), rec.y },
      drawingDetails.blockLength / 10, drawingDetails.GridlineColor);
  }

  for (int j = 0; j < Height; ++j) {
    for (int i = 0; i < Width; ++i) {
      drawBlockPretty(i, j, drawingDetails, getTetrominoColor(grid[j][i]));
    }
  }
}

void Playfield::DrawPieces(const DrawingDetails& drawingDetails) const {
  const FallingPiece ghostPiece = getGhostPiece();
  drawPiece(ghostPiece.tetrominoMap, GRAY, ghostPiece.horizontalPosition, ghostPiece.verticalPosition, drawingDetails);
  drawPiece(fallingPiece.tetrominoMap, getTetrominoColor(fallingPiece.tetromino), fallingPiece.horizontalPosition, fallingPiece.verticalPosition, drawingDetails);
  if (/*inDanger()*/ true) {
    drawPieceDanger({nextQueue[0], InitialHorizontalPosition, InitialVerticalPosition}, drawingDetails);
  }
}

void Playfield::DrawNextComingPieces(const DrawingDetails& drawingDetails) const {
  Rectangle nextTextBlock = getBlockRectangle(Width + 1, VisibleHeight, drawingDetails);
  Rectangle nextQueueBackground = getBlockRectangle(Width + 1, VisibleHeight + 2, drawingDetails);
  nextQueueBackground.width = drawingDetails.blockLength * 6;
  nextQueueBackground.height = drawingDetails.blockLength * (3 * (NextQueue::NextComingSize)+1);
  DrawRectangleRec(nextQueueBackground, drawingDetails.PiecesBackGroundColor);
  DrawRectangleLinesEx(nextQueueBackground, drawingDetails.blockLength / 4, drawingDetails.PieceBoxColor);
  DrawText("NEXT", nextTextBlock.x, nextTextBlock.y, drawingDetails.fontSize, drawingDetails.InfoTextColor);
  for (int id = 0; id < NextQueue::NextComingSize; ++id) {
    Tetromino currentTetromino = nextQueue[id];
    drawPiece(initialTetrominoMap(currentTetromino), getTetrominoColor(currentTetromino),
      Width + 3, 3 * (id + 1) + VisibleHeight + 1, drawingDetails);
  }
}

void Playfield::DrawHoldPiece(const DrawingDetails& drawingDetails) const {
  Rectangle holdTextBlock = getBlockRectangle(-7, VisibleHeight, drawingDetails);
  DrawText("HOLD", holdTextBlock.x, holdTextBlock.y, drawingDetails.fontSize, drawingDetails.InfoTextColor);
  Rectangle holdPieceBackground = getBlockRectangle(-7, VisibleHeight + 2, drawingDetails);
  holdPieceBackground.width = drawingDetails.blockLength * 6;
  holdPieceBackground.height = drawingDetails.blockLength * 4;
  DrawRectangleRec(holdPieceBackground, drawingDetails.PiecesBackGroundColor);
  DrawRectangleLinesEx(holdPieceBackground, drawingDetails.blockLength / 4, drawingDetails.PieceBoxColor);

  Color holdColor = canSwap ? getTetrominoColor(holdingPiece) : drawingDetails.UnavailableHoldPieceColor;
  drawPiece(initialTetrominoMap(holdingPiece), holdColor, -5, 4 + VisibleHeight, drawingDetails);
}

void Playfield::DrawLineClearMessage(const DrawingDetails& drawingDetails) const {
  if (message.timer <= 0) return;

  Rectangle clearTextBlock = getBlockRectangle(drawingDetails.LeftBorder, Height - 4, drawingDetails);
  const float colorScaleFactor = static_cast<float>(message.timer) / LineClearMessage::Duration;
  std::string lineClearMessage;

  Color textColor = BLANK;
  unsigned char alpha = static_cast<unsigned char>(255 * colorScaleFactor);
  switch (message.message) {
  case MessageType::Single:
    lineClearMessage = "SINGLE";
    textColor = { 0, 0, 0, alpha };
    break;
  case MessageType::Double:
    lineClearMessage = "DOUBLE";
    textColor = { 235, 149, 52, alpha };
    break;
  case MessageType::Triple:
    lineClearMessage = "TRIPLE";
    textColor = { 88, 235, 52, alpha };
    break;
  case MessageType::Tetris:
    lineClearMessage = "TETRIS";
    textColor = { 52, 164, 236, alpha };
    break;
  case MessageType::AllClear:
    lineClearMessage = "ALL\nCLEAR";
    textColor = { 235, 52, 213, alpha };
    break;
  case MessageType::Empty:
    lineClearMessage = "";
  }
  DrawText(lineClearMessage.c_str(), clearTextBlock.x, clearTextBlock.y, drawingDetails.fontSize, textColor);

  if (message.spinType == SpinType::No) return;
  Color tSpinTextColor = getTetrominoColor(Tetromino::T);
  tSpinTextColor.a = alpha;
  Rectangle tSpinTextBlock = getBlockRectangle(drawingDetails.LeftBorder, Height - 6, drawingDetails);
  DrawText("TSPIN", tSpinTextBlock.x, tSpinTextBlock.y, drawingDetails.fontSize, tSpinTextColor);
  if (message.spinType == SpinType::Mini) {
    Rectangle miniTSpinTextBlock = getBlockRectangle(drawingDetails.LeftBorder, Height - 7, drawingDetails);
    DrawText("MINI", miniTSpinTextBlock.x, miniTSpinTextBlock.y, drawingDetails.fontSizeSmall, tSpinTextColor);
  }
}

void Playfield::DrawCombo(const DrawingDetails& drawingDetails) const {
  if (combo < 2) return;
  Rectangle comboTextBlock = getBlockRectangle(drawingDetails.LeftBorder, Height - 10, drawingDetails);
  std::string comboText = std::format("{}", combo);
  DrawText("COMBO ", comboTextBlock.x, comboTextBlock.y, drawingDetails.fontSize, BLUE);
  DrawText(comboText.c_str(), comboTextBlock.x + MeasureText("COMBO ", drawingDetails.fontSize), comboTextBlock.y, drawingDetails.fontSize, BLUE);
}

void Playfield::DrawBackToBack(const DrawingDetails& drawingDetails) const {
  if (b2b < 2) return;
  Rectangle b2bTextBlock = getBlockRectangle(drawingDetails.LeftBorder, Height - 12, drawingDetails);
  std::string b2bText = std::format("{}", b2b - 1);
  DrawText("B2B ", b2bTextBlock.x, b2bTextBlock.y, drawingDetails.fontSize, BLUE);
  DrawText(b2bText.c_str(), b2bTextBlock.x + MeasureText("B2B ", drawingDetails.fontSize), b2bTextBlock.y, drawingDetails.fontSize, BLUE);
}

void Playfield::DrawScore(const DrawingDetails& drawingDetails) const {
  Rectangle scoreNumberBlock = getBlockRectangle(Width + 1, Height - 2, drawingDetails);
  std::string scoreText = std::format("{:09}", score);
  DrawText(scoreText.c_str(), scoreNumberBlock.x, scoreNumberBlock.y + drawingDetails.blockLength * 0.5, drawingDetails.fontSize, drawingDetails.InfoTextColor);
}

void Playfield::draw(const DrawingDetails& drawingDetails) const {
  DrawTetrion(drawingDetails);
  DrawPieces(drawingDetails);
  DrawNextComingPieces(drawingDetails);
  DrawHoldPiece(drawingDetails);
  DrawLineClearMessage(drawingDetails);
  DrawCombo(drawingDetails);
  DrawBackToBack(drawingDetails);
  DrawScore(drawingDetails);
}
