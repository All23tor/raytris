#include "Game.hpp"
#include "NextQueue.hpp"
#include "Playfield.hpp"
#include "raylib.h"
#include <format>

namespace { 
  constexpr float HEIGHT_SCALE_FACTOR = 0.80; 
  constexpr Color BACKGROUND_COLOR = LIGHTGRAY;
  constexpr Color DEFAULT_PRETTY_OUTLINE = BLACK;
  constexpr Color TETRION_BACKGROUND_COLOR = BLACK;
  constexpr Color GRIDLINE_COLOR = DARKGRAY;
  constexpr Color UNAVAILABLE_HOLD_PIECE_COLOR = DARKGRAY;
  constexpr Color PIECES_BACKGROUND_COLOR = GRAY;
  constexpr Color INFO_TEXT_COLOR = BLACK;
  constexpr Color PIECE_BOX_COLOR = BLACK;

  constexpr Color YOU_LOST_COLOR = RED;
  constexpr Color GAME_PAUSED_COLOR = BLUE;
  constexpr Color QUIT_COLOR = WHITE;
  constexpr Color DARKEN_COLOR = { 0, 0, 0, 100 };
};

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

Game::Game() :
  blockLength(HEIGHT_SCALE_FACTOR* GetScreenHeight() / (Playfield::VISIBLE_HEIGHT)),
  fontSize(blockLength * 2),
  fontSizeBig(blockLength * 5),
  fontSizeSmall(blockLength),
  position({
    (GetScreenWidth() - blockLength * Playfield::WIDTH) / 2,
    (GetScreenHeight() - blockLength * Playfield::VISIBLE_HEIGHT) / 2 }) {
  undoMoveStack.push(playfield);
}

void Game::DrawRectangleRecPretty(Rectangle rec, Color fill, Color outline = DEFAULT_PRETTY_OUTLINE) const {
  if (fill.a == 0)
    return;

  outline.a /= 8;
  DrawRectangleRec(rec, fill);
  DrawRectangle(rec.x + blockLength / 3, rec.y + blockLength / 3, rec.width / 3,
    rec.height / 3, outline);
  DrawRectangleLinesEx(rec, blockLength / 8, outline);
}

inline Rectangle Game::getBlockRectangle(int i, int j) const {
  return { position.x + i * blockLength,
           position.y + (j - static_cast<int>(Playfield::VISIBLE_HEIGHT)) * blockLength,
           blockLength, blockLength };
}

void Game::update() {
  if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)) {
    if (!undoMoveStack.empty()) {
      playfield = undoMoveStack.top();
      undoMoveStack.pop();
      if (undoMoveStack.empty()) undoMoveStack.push(playfield);
      return;
    }
  }

  if (IsKeyPressed(KEY_ENTER)) paused = !paused;
  if (paused) return;
  if (playfield.update()) undoMoveStack.push(playfield);
}

void Game::DrawTetrion() const {
  Rectangle tetrion = Rectangle{
      position.x, position.y,
      blockLength * Playfield::WIDTH, blockLength * Playfield::VISIBLE_HEIGHT };
  DrawRectangleRec(tetrion, TETRION_BACKGROUND_COLOR);
  DrawRectangleLinesEx(tetrion, blockLength / 10, GRIDLINE_COLOR);

  for (int i = 1; i < Playfield::WIDTH; ++i) {
    Rectangle rec = getBlockRectangle(i, Playfield::VISIBLE_HEIGHT);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx({ rec.x, rec.y },
      { rec.x, std::floor(rec.y + Playfield::VISIBLE_HEIGHT * blockLength) },
      blockLength / 10, GRIDLINE_COLOR);
  }

  for (int j = 1; j < Playfield::VISIBLE_HEIGHT; ++j) {
    Rectangle rec = getBlockRectangle(0, j + Playfield::VISIBLE_HEIGHT);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx({ rec.x, rec.y },
      { std::floor(rec.x + blockLength * Playfield::WIDTH), rec.y },
      blockLength / 10, GRIDLINE_COLOR);
  }

  for (int j = 0; j < Playfield::HEIGHT; ++j) {
    for (int i = 0; i < Playfield::WIDTH; ++i) {
      DrawRectangleRecPretty(getBlockRectangle(i, j),
        getTetrominoColor(playfield.grid[j][i]));
    }
  }
}

void Game::DrawPieces() const {
  const FallingPiece ghostPiece = playfield.getGhostPiece();
  drawPiece(ghostPiece.tetrominoMap, GRAY, ghostPiece.horizontalPosition, ghostPiece.verticalPosition);
  const FallingPiece& fallingPiece = playfield.fallingPiece;
  drawPiece(fallingPiece.tetrominoMap, getTetrominoColor(fallingPiece.tetromino),
    fallingPiece.horizontalPosition, fallingPiece.verticalPosition);
}

void Game::drawPiece(const TetrominoMap& map, Color color, int horizontalOffset, int verticalOffset) const {
  for (const CoordinatePair& coordinates : map) {
    int i = coordinates.x + horizontalOffset;
    int j = coordinates.y + verticalOffset;
    DrawRectangleRecPretty(getBlockRectangle(i, j), color);
  }
}

void Game::DrawNextComingPieces() const {
  Rectangle nextTextBlock = getBlockRectangle(Playfield::WIDTH + 1, Playfield::VISIBLE_HEIGHT);
  Rectangle nextQueueBackground = getBlockRectangle(Playfield::WIDTH + 1, Playfield::VISIBLE_HEIGHT + 2);
  nextQueueBackground.width = blockLength * 6;
  nextQueueBackground.height = blockLength * (3 * (NextQueue::NEXT_COMING_SIZE)+1);
  DrawRectangleRec(nextQueueBackground, PIECES_BACKGROUND_COLOR);
  DrawRectangleLinesEx(nextQueueBackground, blockLength / 4, PIECE_BOX_COLOR);
  DrawText("NEXT", nextTextBlock.x, nextTextBlock.y, fontSize, INFO_TEXT_COLOR);
  for (int id = 0; id < NextQueue::NEXT_COMING_SIZE; ++id) {
    Tetromino currentTetromino = playfield.nextQueue[id];
    drawPiece(initialTetrominoMap(currentTetromino), getTetrominoColor(currentTetromino),
      Playfield::WIDTH + 3, 3 * (id + 1) + Playfield::VISIBLE_HEIGHT + 1);
  }
}

void Game::DrawHoldPiece() const {
  Rectangle holdTextBlock = getBlockRectangle(-7, Playfield::VISIBLE_HEIGHT);
  DrawText("HOLD", holdTextBlock.x, holdTextBlock.y, fontSize, INFO_TEXT_COLOR);
  Rectangle holdPieceBackground = getBlockRectangle(-7, Playfield::VISIBLE_HEIGHT + 2);
  holdPieceBackground.width = blockLength * 6;
  holdPieceBackground.height = blockLength * 4;
  DrawRectangleRec(holdPieceBackground, PIECES_BACKGROUND_COLOR);
  DrawRectangleLinesEx(holdPieceBackground, blockLength / 4, PIECE_BOX_COLOR);

  Color holdColor = playfield.canSwap ? getTetrominoColor(playfield.holdingPiece) : UNAVAILABLE_HOLD_PIECE_COLOR;
  drawPiece(initialTetrominoMap(playfield.holdingPiece), holdColor, -5, 4 + Playfield::VISIBLE_HEIGHT);
}

void Game::DrawLineClearMessage() const {
  if (playfield.message.timer <= 0) return;

  Rectangle clearTextBlock = getBlockRectangle(LEFT_BORDER, Playfield::HEIGHT - 4);
  const float colorScaleFactor = static_cast<float>(playfield.message.timer) / LineClearMessage::DURATION;
  std::string lineClearMessage;

  Color textColor = BLANK;
  unsigned char alpha = static_cast<unsigned char>(255 * colorScaleFactor);
  switch (playfield.message.message) {
  case MessageType::SINGLE:
    lineClearMessage = "SINGLE";
    textColor = { 0, 0, 0, alpha };
    break;
  case MessageType::DOUBLE:
    lineClearMessage = "DOUBLE";
    textColor = { 235, 149, 52, alpha };
    break;
  case MessageType::TRIPLE:
    lineClearMessage = "TRIPLE";
    textColor = { 88, 235, 52, alpha };
    break;
  case MessageType::TETRIS:
    lineClearMessage = "TETRIS";
    textColor = { 52, 164, 236, alpha };
    break;
  case MessageType::ALLCLEAR:
    lineClearMessage = "ALL\nCLEAR";
    textColor = { 235, 52, 213, alpha };
    break;
  case MessageType::EMPTY:
    lineClearMessage = "";
  }
  DrawText(lineClearMessage.c_str(), clearTextBlock.x, clearTextBlock.y, fontSize, textColor);

  if (playfield.message.spinType == SpinType::No) return;
  Color tSpinTextColor = getTetrominoColor(Tetromino::T);
  tSpinTextColor.a = alpha;
  Rectangle tSpinTextBlock = getBlockRectangle(LEFT_BORDER, Playfield::HEIGHT - 6);
  DrawText("TSPIN", tSpinTextBlock.x, tSpinTextBlock.y, fontSize, tSpinTextColor);
  if (playfield.message.spinType == SpinType::Mini) {
    Rectangle miniTSpinTextBlock = getBlockRectangle(LEFT_BORDER, Playfield::HEIGHT - 7);
    DrawText("MINI", miniTSpinTextBlock.x, miniTSpinTextBlock.y, fontSizeSmall, tSpinTextColor);
  }
}

void Game::DrawCombo() const {
  if (playfield.combo < 2) return;
  Rectangle comboTextBlock = getBlockRectangle(LEFT_BORDER, Playfield::HEIGHT - 10);
  std::string combo = std::format("{}", playfield.combo);
  DrawText("COMBO ", comboTextBlock.x, comboTextBlock.y, fontSize, BLUE);
  DrawText(combo.c_str(), comboTextBlock.x + MeasureText("COMBO ", fontSize), comboTextBlock.y, fontSize, BLUE);
}

void Game::DrawBackToBack() const {
  if (playfield.b2b < 2) return;
  Rectangle b2bTextBlock = getBlockRectangle(LEFT_BORDER, Playfield::HEIGHT - 12);
  std::string b2b = std::format("{}", playfield.b2b - 1);
  DrawText("B2B ", b2bTextBlock.x, b2bTextBlock.y, fontSize, BLUE);
  DrawText(b2b.c_str(), b2bTextBlock.x + MeasureText("B2B ", fontSize), b2bTextBlock.y, fontSize, BLUE);
}

void Game::DrawScore() const {
  Rectangle scoreNumberBlock = getBlockRectangle(Playfield::WIDTH + 1, Playfield::HEIGHT - 2);
  std::string score = std::format("{:09}", playfield.score);
  DrawText(score.c_str(), scoreNumberBlock.x, scoreNumberBlock.y + blockLength * 0.5, fontSize, INFO_TEXT_COLOR);
}

void Game::DrawPauseMenu() const {
  if (!playfield.hasLost && !paused) return;

  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  DrawRectangle(0, 0, screenWidth, screenHeight, DARKEN_COLOR);

  if (playfield.hasLost) {
    DrawText("YOU LOST",
      (screenWidth - MeasureText("YOU LOST", fontSizeBig)) / 2.0,
      screenHeight / 2.0, fontSizeBig, YOU_LOST_COLOR);

  } else if (paused) {
    DrawText("GAME PAUSED",
      (screenWidth - MeasureText("GAME PAUSED", fontSizeBig)) / 2.0,
      screenHeight / 2.0, fontSizeBig, GAME_PAUSED_COLOR);
  }
  DrawText("Press Esc to quit",
    (screenWidth - MeasureText("Press Enter to quit", fontSize)) / 2.0,
    screenHeight / 2.0 + fontSizeBig, fontSize, QUIT_COLOR);
}

void Game::draw() const {
  ClearBackground(BACKGROUND_COLOR);
  DrawTetrion();
  DrawPieces();
  DrawNextComingPieces();
  DrawHoldPiece();
  DrawLineClearMessage();
  DrawCombo();
  DrawBackToBack();
  DrawScore();
  DrawPauseMenu();
}

void Game::run() {
  while (!IsKeyPressed(KEY_ESCAPE) || !(paused || playfield.hasLost)) {
    update();
    BeginDrawing();
    draw();
    EndDrawing();
  }

  BeginDrawing();
  EndDrawing();
}
