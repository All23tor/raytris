#include "Game.hpp"
#include "NextQueue.hpp"
#include "Playfield.hpp"
#include "raylib.h"
#include <format>

static constexpr float heightScaleFactor = 0.8;

Game::Game() :
  blockLength(heightScaleFactor* GetScreenHeight() / Playfield::VISIBLE_HEIGHT),
  position({
    (GetScreenWidth() - blockLength * Playfield::WIDTH) / 2,
    (GetScreenHeight() - blockLength * Playfield::VISIBLE_HEIGHT) / 2 }) {
  undoMoveStack.push(playfield);
}

void Game::DrawRectangleRecPretty(Rectangle rec, Color fill, Color outline = BLACK) const {
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
  DrawRectangleRec(tetrion, BLACK);
  DrawRectangleLinesEx(tetrion, blockLength / 10, DARKGRAY);

  for (int i = 1; i < Playfield::WIDTH; ++i) {
    Rectangle rec = getBlockRectangle(i, Playfield::VISIBLE_HEIGHT);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx({ rec.x, rec.y },
      { rec.x, std::floor(rec.y + Playfield::VISIBLE_HEIGHT * blockLength) },
      blockLength / 10, DARKGRAY);
  }

  for (int j = 1; j < Playfield::VISIBLE_HEIGHT; ++j) {
    Rectangle rec = getBlockRectangle(0, j + Playfield::VISIBLE_HEIGHT);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx({ rec.x, rec.y },
      { std::floor(rec.x + blockLength * Playfield::WIDTH), rec.y },
      blockLength / 10, DARKGRAY);
  }

  for (int j = 0; j < Playfield::HEIGHT; ++j) {
    for (int i = 0; i < Playfield::WIDTH; ++i) {
      DrawRectangleRecPretty(getBlockRectangle(i, j),
        getTetrominoColor(playfield.grid[j][i]));
    }
  }
}

void Game::drawPiece(const TetrominoMap& map, Color color, int horizontalOffset, int verticalOffset) const {
  for (const CoordinatePair& coordinates : map) {
    int i = coordinates.x + horizontalOffset;
    int j = coordinates.y + verticalOffset;
    DrawRectangleRecPretty(getBlockRectangle(i, j), color);
  }
}

void Game::draw() const {
  ClearBackground(LIGHTGRAY);

  DrawTetrion();

  const FallingPiece ghostPiece = playfield.getGhostPiece();
  drawPiece(ghostPiece.tetrominoMap, GRAY, ghostPiece.horizontalPosition, ghostPiece.verticalPosition);
  const FallingPiece& fallingPiece = playfield.fallingPiece;
  drawPiece(fallingPiece.tetrominoMap, getTetrominoColor(fallingPiece.tetromino),
    fallingPiece.horizontalPosition, fallingPiece.verticalPosition);

  const int fontSize = blockLength * 2;
  // Next coming pieces
  Rectangle nextTextBlock =
    getBlockRectangle(Playfield::WIDTH + 1, Playfield::VISIBLE_HEIGHT);
  Rectangle nextQueueBackground =
    getBlockRectangle(Playfield::WIDTH + 1, Playfield::VISIBLE_HEIGHT + 2);
  nextQueueBackground.width = blockLength * 6;
  nextQueueBackground.height = blockLength * (3 * (NextQueue::NEXT_COMING_SIZE)+1);
  DrawRectangleRec(nextQueueBackground, GRAY);
  DrawRectangleLinesEx(nextQueueBackground, blockLength / 4, BLACK);
  DrawText("NEXT", nextTextBlock.x, nextTextBlock.y, fontSize, BLACK);
  for (int id = 0; id < NextQueue::NEXT_COMING_SIZE; ++id) {
    Tetromino currentTetromino = playfield.nextQueue[id];
    drawPiece(initialTetrominoMap(currentTetromino), getTetrominoColor(currentTetromino),
      Playfield::WIDTH + 3, 3 * (id + 1) + Playfield::VISIBLE_HEIGHT + 1);
  }

  // Draw hold piece
  Rectangle holdTextBlock = getBlockRectangle(-7, Playfield::VISIBLE_HEIGHT);
  DrawText("HOLD", holdTextBlock.x, holdTextBlock.y, fontSize, BLACK);
  Rectangle holdPieceBackground =
    getBlockRectangle(-7, Playfield::VISIBLE_HEIGHT + 2);
  holdPieceBackground.width = blockLength * 6;
  holdPieceBackground.height = blockLength * 4;
  DrawRectangleRec(holdPieceBackground, GRAY);
  DrawRectangleLinesEx(holdPieceBackground, blockLength / 4, BLACK);

  Color holdColor = playfield.canSwap ? getTetrominoColor(playfield.holdingPiece) : DARKGRAY;
  drawPiece(initialTetrominoMap(playfield.holdingPiece), holdColor, -5, 4 + Playfield::VISIBLE_HEIGHT);

  // Line Clear message
  if (playfield.message.timer > 0) {
    Rectangle clearTextBlock = getBlockRectangle(-10, Playfield::HEIGHT);
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
      lineClearMessage = "ALL CLEAR";
      textColor = { 235, 52, 213, alpha };
      break;
    case MessageType::EMPTY:
      lineClearMessage = "";
    }
    DrawText(lineClearMessage.c_str(), clearTextBlock.x, clearTextBlock.y, fontSize, textColor);

    Color tSpinTextColor = getTetrominoColor(Tetromino::T);
    tSpinTextColor.a = alpha;
    Rectangle tSpinTextBlock = getBlockRectangle(-10, Playfield::HEIGHT - 2);
    if (playfield.message.spinType == SpinType::Proper) {
      DrawText("TSPIN", tSpinTextBlock.x, tSpinTextBlock.y, fontSize, tSpinTextColor);
    } else if (playfield.message.spinType == SpinType::Mini) {
      DrawText("TSPIN MINI", tSpinTextBlock.x, tSpinTextBlock.y, fontSize, tSpinTextColor);
    }
  }


  // Combo
  if (playfield.combo >= 2) {
    Rectangle comboTextBlock = getBlockRectangle(-10, Playfield::HEIGHT - 6);
    std::string combo = std::format("{}", playfield.combo);
    DrawText("COMBO ", comboTextBlock.x, comboTextBlock.y, fontSize, BLUE);
    DrawText(combo.c_str(), comboTextBlock.x + MeasureText("COMBO ", fontSize), comboTextBlock.y, fontSize, BLUE);
  }

  // Back to Back (B2B)
  if (playfield.b2b >= 2) {
    Rectangle b2bTextBlock = getBlockRectangle(-10, Playfield::HEIGHT - 8);
    std::string b2b = std::format("{}", playfield.b2b - 1);
    DrawText("B2B ", b2bTextBlock.x, b2bTextBlock.y, fontSize, BLUE);
    DrawText(b2b.c_str(), b2bTextBlock.x + MeasureText("B2B ", fontSize), b2bTextBlock.y, fontSize, BLUE);
  }

  // Score
  Rectangle scoreTextBlock = getBlockRectangle(Playfield::WIDTH + 1, Playfield::HEIGHT - 2);
  DrawText("SCORE: ", scoreTextBlock.x, scoreTextBlock.y, fontSize, BLACK);
  Rectangle scoreNumberBlock = getBlockRectangle(Playfield::WIDTH + 1, Playfield::HEIGHT);
  std::string score = std::format("{:09}", playfield.score);
  DrawText(score.c_str(), scoreNumberBlock.x, scoreNumberBlock.y, fontSize, BLACK);

  if (!playfield.hasLost && !paused) return;

  // Game over or paused
  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  const int fontSizeBig = blockLength * 5;
  DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 100 });

  if (playfield.hasLost) {
    DrawText("YOU LOST",
      (screenWidth - MeasureText("YOU LOST", fontSizeBig)) / 2.0,
      screenHeight / 2.0, fontSizeBig, RED);

  } else if (paused) {
    DrawText("GAME PAUSED",
      (screenWidth - MeasureText("GAME PAUSED", fontSizeBig)) / 2.0,
      screenHeight / 2.0, fontSizeBig, BLUE);
  }
  DrawText("Press Esc to quit",
    (screenWidth - MeasureText("Press Enter to quit", fontSize)) / 2.0,
    screenHeight / 2.0 + fontSizeBig, fontSize, WHITE);
}

void Game::run() {
  while (!IsKeyPressed(KEY_ESCAPE) || !paused) {
    update();
    BeginDrawing();
    draw();
    EndDrawing();
  }

  BeginDrawing();
  EndDrawing();
}
