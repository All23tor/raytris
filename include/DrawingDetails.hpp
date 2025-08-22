#ifndef DRAWING_DETAILS_H
#define DRAWING_DETAILS_H

#include "raylib.h"

struct DrawingDetails {
  static constexpr float HEIGHT_SCALE_FACTOR = 0.80;
  static constexpr Color DEFAULT_PRETTY_OUTLINE = {0, 0, 0, 255 / 8};
  static constexpr Color TETRION_BACKGROUND_COLOR = BLACK;
  static constexpr Color GRINDLINE_COLOR = DARKGRAY;
  static constexpr Color UNAVAILABLE_HOLD_PIECE_COLOR = DARKGRAY;
  static constexpr Color PIECES_BACKGROUND_COLOR = GRAY;
  static constexpr Color INFO_TEXT_COLOR = BLACK;
  static constexpr Color PIECE_BOX_COLOR = BLACK;
  static constexpr Color YOU_LOST_COLOR = RED;
  static constexpr Color GAME_PAUSED_COLOR = BLUE;
  static constexpr Color QUIT_COLOR = WHITE;
  static constexpr Color DARKEN_COLOR = {0, 0, 0, 100};
  static constexpr Color BACKGROUND_COLOR = LIGHTGRAY;
  static constexpr int LEFT_BORDER = -10;

  const float blockLength;
  const Vector2 position;
  const int fontSize;
  const int fontSizeBig;
  const int fontSizeSmall;

  DrawingDetails(float _blockLength, Vector2 _position) :
    blockLength(_blockLength),
    fontSize(blockLength * 2),
    fontSizeBig(blockLength * 5),
    fontSizeSmall(blockLength),
    position{_position} {}
};

#endif
