#include "DrawingDetails.hpp"
#include "Playfield.hpp"

DrawingDetails::DrawingDetails(float _blockLength, Vector2 _position):
  blockLength(_blockLength),
  fontSize(blockLength * 2),
  fontSizeBig(blockLength * 5),
  fontSizeSmall(blockLength),
  position{_position.x - blockLength * Playfield::Width / 2, _position.y - blockLength * Playfield::VisibleHeight / 2} {}
