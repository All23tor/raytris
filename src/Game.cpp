#include "Game.hpp"
#include "raylib.h"

Game::Game()
    : position(
          {(GetScreenWidth() - block_length * Playfield::WIDTH) / 2,
           (GetScreenHeight() -
            block_length * (Playfield::HEIGHT + Playfield::VISIBLE_HEIGHT)) /
               2}),
      block_length(GetScreenHeight() * 0.7 / Playfield::VISIBLE_HEIGHT) {
  undoMoveStack.push(playfield);
}

inline Rectangle Game::getBlockRectangle(int i, int j) {
  return {position.x + i * block_length, position.y + j * block_length,
          block_length, block_length};
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

void Game::draw() {
  ClearBackground(LIGHTGRAY);

  DrawRectangle(position.x,
                position.y + block_length * Playfield::VISIBLE_HEIGHT,
                block_length * Playfield::WIDTH,
                block_length * Playfield::VISIBLE_HEIGHT, BLACK);

  for (int j = 0; j < Playfield::HEIGHT; ++j) {
    for (int i = 0; i < Playfield::WIDTH; ++i) {
      DrawRectangleRec(getBlockRectangle(i, j),
                       getTetrominoColor(playfield.grid[j][i]));
    }
  }

  // Ghost piece calculation
  FallingPiece ghostPiece = playfield.getGhostPiece();
  // Ghost piece drawing
  for (const CoordinatePair &coordinates : ghostPiece.tetromino_map) {
    int i = coordinates.x + ghostPiece.horizontal_position;
    int j = coordinates.y + ghostPiece.vertical_position;
    DrawRectangleRec(getBlockRectangle(i, j), (Color){100, 100, 100, 255});
  }
  // Falling piece drawing
  for (const CoordinatePair &coordinates :
       playfield.fallingPiece.tetromino_map) {
    int i = coordinates.x + playfield.fallingPiece.horizontal_position;
    int j = coordinates.y + playfield.fallingPiece.vertical_position;
    DrawRectangleRec(getBlockRectangle(i, j),
                     getTetrominoColor(playfield.fallingPiece.tetromino));
  }

  const int fontSize = block_length * 2;
  // Next coming pieces
  Rectangle next_text_block =
      getBlockRectangle(Playfield::WIDTH + 1, Playfield::VISIBLE_HEIGHT);
  DrawText("NEXT", next_text_block.x, next_text_block.y, fontSize, BLACK);
  for (int id = 0; id < NextQueue::NEXT_COMING_SIZE; ++id) {
    Tetromino currentTetromino = playfield.next_queue[id];
    TetrominoMap currentTetrominoMap = initialTetrominoMap(currentTetromino);
    for (const CoordinatePair &coordinates : currentTetrominoMap) {
      int horizontal_offset = Playfield::WIDTH + 2;
      int vertical_offset = 3 * (id + 1) + Playfield::VISIBLE_HEIGHT;
      int i = coordinates.x + horizontal_offset;
      int j = coordinates.y + vertical_offset;
      DrawRectangleRec(getBlockRectangle(i, j),
                       getTetrominoColor(currentTetromino));
    }
  }

  // Draw hold piece
  Rectangle hold_text_block = getBlockRectangle(-6, Playfield::VISIBLE_HEIGHT);
  DrawText("HOLD", hold_text_block.x, hold_text_block.y, fontSize, BLACK);
  Color hold_color =
      playfield.canSwap ? getTetrominoColor(playfield.holdingPiece) : DARKGRAY;
  TetrominoMap hold_tetromino_map = initialTetrominoMap(playfield.holdingPiece);
  for (const CoordinatePair &coordinates : hold_tetromino_map) {
    int i = coordinates.x - 3;
    int j = coordinates.y + 3 + Playfield::VISIBLE_HEIGHT;
    DrawRectangleRec(getBlockRectangle(i, j), hold_color);
  }

  // Line Clear message
  if (playfield.message.timer > 0) {
    Rectangle clear_text_block = getBlockRectangle(-5, Playfield::HEIGHT);
    const float color_scale_factor =
        (float)playfield.message.timer / LineClearMessage::DURATION;
    const char *text_message;

    Color text_color = {0, 0, 0,
                        static_cast<unsigned char>(255 * color_scale_factor)};
    switch (playfield.message.message) {
    case MessageType::SINGLE:
      text_message = "SINGLE";
      break;
    case MessageType::DOUBLE:
      text_message = "DOUBLE";
      text_color = (Color){235, 149, 52, text_color.a};
      break;
    case MessageType::TRIPLE:
      text_message = "TRIPLE";
      text_color = (Color){88, 235, 52, text_color.a};
      break;
    case MessageType::TETRIS:
      text_message = "TETRIS";
      text_color = (Color){52, 164, 236, text_color.a};
      break;
    case MessageType::ALLCLEAR:
      text_message = "ALL CLEAR";
      text_color = (Color){235, 52, 213, text_color.a};
      break;
    case MessageType::EMPTY_MESSAGE:
      text_message = "";
    }
    DrawText(text_message, clear_text_block.x, clear_text_block.y, fontSize,
             text_color);
  }

  // Combo
  if (playfield.combo >= 2) {
    Rectangle combo_text_block = getBlockRectangle(-5, Playfield::HEIGHT + 2);
    char str_score[5];
    snprintf(str_score, sizeof str_score, "%d", playfield.combo);
    DrawText("COMBO ", combo_text_block.x, combo_text_block.y, fontSize, BLUE);
    DrawText(str_score, combo_text_block.x + MeasureText("COMBO ", fontSize),
             combo_text_block.y, fontSize, BLUE);
  }

  // Back to Back (B2B)
  if (playfield.b2b >= 2) {
    Rectangle b2b_text_block = getBlockRectangle(-7, Playfield::HEIGHT - 2);
    char str_score[5];
    snprintf(str_score, sizeof str_score, "%d", playfield.b2b - 1);
    DrawText("B2B ", b2b_text_block.x, b2b_text_block.y, fontSize, BLUE);
    DrawText(str_score, b2b_text_block.x + MeasureText("B2B ", fontSize),
             b2b_text_block.y, fontSize, BLUE);
  }

  // Score
  Rectangle score_text_block =
      getBlockRectangle(Playfield::WIDTH + 1, Playfield::HEIGHT);
  char str_score[11];
  snprintf(str_score, sizeof str_score, "%08ld", playfield.score);
  DrawText("SCORE: ", score_text_block.x, score_text_block.y, fontSize, BLACK);
  DrawText(str_score, score_text_block.x + MeasureText("SCORE: ", fontSize),
           score_text_block.y, fontSize, BLACK);

  if (!playfield.hasLost && !paused)
    return;
  // Game over or paused
  float screenWidth = GetScreenWidth();
  float screenHeight = GetScreenHeight();
  int fontSizeBig = block_length * 5;
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
