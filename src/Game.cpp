#include "Game.hpp"
#include "NextQueue.hpp"
#include "Playfield.hpp"
#include "raylib.h"

static constexpr float heightScaleFactor = 0.8;

Game::Game()
    : blockLength(GetScreenHeight() * heightScaleFactor /
                  Playfield::VISIBLE_HEIGHT),
      position(
          {(GetScreenWidth() - blockLength * Playfield::WIDTH) / 2,
           (GetScreenHeight() -
            blockLength * (Playfield::HEIGHT + Playfield::VISIBLE_HEIGHT)) /
               2}) {
  undoMoveStack.push(playfield);
}

void Game::DrawRectangleRecPretty(Rectangle rec, Color fill,
                                  Color outline = BLACK) const {

  if (fill.a == 0)
    return;

  outline.a /= 8;
  DrawRectangleRec(rec, fill);
  DrawRectangle(rec.x + blockLength / 3, rec.y + blockLength / 3, rec.width / 3,
                rec.height / 3, outline);
  DrawRectangleLinesEx(rec, blockLength / 8, outline);
}

inline Rectangle Game::getBlockRectangle(int i, int j) const {
  return {position.x + i * blockLength, position.y + j * blockLength,
          blockLength, blockLength};
}

void Game::update() {
  if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)) {
    if (!undoMoveStack.empty()) {
      playfield = undoMoveStack.top();
      undoMoveStack.pop();
      return;
    }
  }

  if (IsKeyPressed(KEY_ENTER))
    paused = !paused;

  if (paused)
    return;

  if (playfield.update())
    undoMoveStack.push(playfield);
}

void Game::DrawTetrion() const {
  Rectangle tetrion = Rectangle{
      position.x, position.y + blockLength * Playfield::VISIBLE_HEIGHT,
      blockLength * Playfield::WIDTH, blockLength * Playfield::VISIBLE_HEIGHT};
  DrawRectangleRec(tetrion, BLACK);

  for (int i = 1; i < Playfield::WIDTH; ++i) {
    Rectangle rec = getBlockRectangle(i, Playfield::VISIBLE_HEIGHT);
    DrawLineEx({rec.x, rec.y},
               {rec.x, rec.y + Playfield::VISIBLE_HEIGHT * blockLength},
               blockLength / 20, DARKGRAY);
  }

  for (int j = 1; j < Playfield::VISIBLE_HEIGHT; ++j) {
    Rectangle rec = getBlockRectangle(0, j + Playfield::VISIBLE_HEIGHT);
    DrawLineEx({rec.x, rec.y}, {rec.x + blockLength * Playfield::WIDTH, rec.y},
               blockLength / 20, DARKGRAY);
  }

  for (int j = 0; j < Playfield::HEIGHT; ++j) {
    for (int i = 0; i < Playfield::WIDTH; ++i) {
      DrawRectangleRecPretty(getBlockRectangle(i, j),
                             getTetrominoColor(playfield.grid[j][i]));
    }
  }
}

void Game::draw() const {
  ClearBackground(LIGHTGRAY);

  DrawTetrion();

  // Ghost piece calculation
  FallingPiece ghostPiece = playfield.getGhostPiece();
  // Ghost piece drawing
  for (const CoordinatePair &coordinates : ghostPiece.tetrominoMap) {
    int i = coordinates.x + ghostPiece.horizontalPosition;
    int j = coordinates.y + ghostPiece.verticalPosition;
    DrawRectangleRecPretty(getBlockRectangle(i, j), GRAY);
  }
  // Falling piece drawing
  for (const CoordinatePair &coordinates :
       playfield.fallingPiece.tetrominoMap) {
    int i = coordinates.x + playfield.fallingPiece.horizontalPosition;
    int j = coordinates.y + playfield.fallingPiece.verticalPosition;
    DrawRectangleRecPretty(getBlockRectangle(i, j),
                           getTetrominoColor(playfield.fallingPiece.tetromino));
  }

  const int fontSize = blockLength * 2;
  // Next coming pieces
  Rectangle nextTextBlock =
      getBlockRectangle(Playfield::WIDTH + 1, Playfield::VISIBLE_HEIGHT);
  Rectangle nextQueueBackground =
      getBlockRectangle(Playfield::WIDTH + 1, Playfield::VISIBLE_HEIGHT + 2);
  nextQueueBackground.width = blockLength * 6;
  nextQueueBackground.height =
      blockLength * (3 * (NextQueue::NEXT_COMING_SIZE) + 1);
  DrawRectangleRec(nextQueueBackground, GRAY);
  DrawRectangleLinesEx(nextQueueBackground, blockLength / 4, BLACK);
  DrawText("NEXT", nextTextBlock.x, nextTextBlock.y, fontSize, BLACK);
  for (int id = 0; id < NextQueue::NEXT_COMING_SIZE; ++id) {
    Tetromino currentTetromino = playfield.nextQueue[id];
    TetrominoMap currentTetrominoMap = initialTetrominoMap(currentTetromino);
    for (const CoordinatePair &coordinates : currentTetrominoMap) {
      int horizontalOffset = Playfield::WIDTH + 3;
      int verticalOffset = 3 * (id + 1) + Playfield::VISIBLE_HEIGHT + 1;
      int i = coordinates.x + horizontalOffset;
      int j = coordinates.y + verticalOffset;
      DrawRectangleRecPretty(getBlockRectangle(i, j),
                             getTetrominoColor(currentTetromino));
    }
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
  Color hold_color =
      playfield.canSwap ? getTetrominoColor(playfield.holdingPiece) : DARKGRAY;
  TetrominoMap hold_tetromino_map = initialTetrominoMap(playfield.holdingPiece);
  for (const CoordinatePair &coordinates : hold_tetromino_map) {
    int i = coordinates.x - 5;
    int j = coordinates.y + 4 + Playfield::VISIBLE_HEIGHT;
    DrawRectangleRecPretty(getBlockRectangle(i, j), hold_color);
  }

  // Line Clear message
  if (playfield.message.timer > 0) {
    Rectangle clearTextBlock = getBlockRectangle(-10, Playfield::HEIGHT);
    const float colorScaleFactor =
        (float)playfield.message.timer / LineClearMessage::DURATION;
    const char *textMessage;

    Color textColor = {0, 0, 0,
                       static_cast<unsigned char>(255 * colorScaleFactor)};
    switch (playfield.message.message) {
    case MessageType::SINGLE:
      textMessage = "SINGLE";

      break;
    case MessageType::DOUBLE:
      textMessage = "DOUBLE";
      textColor = (Color){235, 149, 52, textColor.a};
      break;
    case MessageType::TRIPLE:
      textMessage = "TRIPLE";
      textColor = (Color){88, 235, 52, textColor.a};
      break;
    case MessageType::TETRIS:
      textMessage = "TETRIS";
      textColor = (Color){52, 164, 236, textColor.a};
      break;
    case MessageType::ALLCLEAR:
      textMessage = "ALL CLEAR";
      textColor = (Color){235, 52, 213, textColor.a};
      break;
    case MessageType::EMPTY_MESSAGE:
      textMessage = "";
    }
    DrawText(textMessage, clearTextBlock.x, clearTextBlock.y, fontSize,
             textColor);
  }

  // Combo
  if (playfield.combo >= 2) {
    Rectangle comboTextBlock = getBlockRectangle(-10, Playfield::HEIGHT - 2);
    char strCombo[5];
    snprintf(strCombo, sizeof strCombo, "%d", playfield.combo);
    DrawText("COMBO ", comboTextBlock.x, comboTextBlock.y, fontSize, BLUE);
    DrawText(strCombo, comboTextBlock.x + MeasureText("COMBO ", fontSize),
             comboTextBlock.y, fontSize, BLUE);
  }

  // Back to Back (B2B)
  if (playfield.b2b >= 2) {
    Rectangle b2bTextBlock = getBlockRectangle(-10, Playfield::HEIGHT - 4);
    char strB2b[5];
    snprintf(strB2b, sizeof strB2b, "%d", playfield.b2b - 1);
    DrawText("B2B ", b2bTextBlock.x, b2bTextBlock.y, fontSize, BLUE);
    DrawText(strB2b, b2bTextBlock.x + MeasureText("B2B ", fontSize),
             b2bTextBlock.y, fontSize, BLUE);
  }

  // Score
  Rectangle scoreTextBlock = getBlockRectangle(11, Playfield::HEIGHT - 2);
  DrawText("SCORE: ", scoreTextBlock.x, scoreTextBlock.y, fontSize, BLACK);
  Rectangle scoreNumberBlock = getBlockRectangle(11, Playfield::HEIGHT);
  char strScore[10];
  snprintf(strScore, sizeof strScore, "%09ld", playfield.score);
  DrawText(strScore, scoreNumberBlock.x, scoreNumberBlock.y, fontSize, BLACK);

  if (!playfield.hasLost && !paused)
    return;

  // Game over or paused
  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  const int fontSizeBig = blockLength * 5;
  DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 100});

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
}
