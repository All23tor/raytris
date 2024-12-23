#ifndef DRAWING_DETAILS_H
#define DRAWING_DETAILS_H

#include "raylib.h"

struct DrawingDetails {
  static constexpr float HeightScaleFactor = 0.80;
  static constexpr Color DefaultPrettyOutline = BLACK;
  static constexpr Color TetrionBackgroundColor = BLACK;
  static constexpr Color GridlineColor = DARKGRAY;
  static constexpr Color UnavailableHoldPieceColor = DARKGRAY;
  static constexpr Color PiecesBackGroundColor = GRAY;
  static constexpr Color InfoTextColor = BLACK;
  static constexpr Color PieceBoxColor = BLACK;
  static constexpr Color YouLostColor = RED;
  static constexpr Color GamePausedColor = BLUE;
  static constexpr Color QuitColor = WHITE;
  static constexpr Color DarkenColor = { 0, 0, 0, 100 };
  static constexpr Color BackgroundColor = LIGHTGRAY;
  static constexpr int LeftBorder = -10;

  const float blockLength;
  const Vector2 position;
  const int fontSize;
  const int fontSizeBig;
  const int fontSizeSmall;

  DrawingDetails(float _blockLength, Vector2 _position);
};

#endif