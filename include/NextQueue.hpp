#ifndef NEXT_QUEUE_H
#define NEXT_QUEUE_H

#include "FallingPiece.hpp"

class NextQueue {
public:
  static constexpr std::size_t NEXT_SIZE = 5;
  static constexpr std::size_t SIZE_OF_BAG = 7;
  static constexpr std::size_t MAX_QUEUE_SIZE = NEXT_SIZE + SIZE_OF_BAG;

private:
  std::array<Tetromino, MAX_QUEUE_SIZE> queue;
  std::size_t queue_size;

public:
  NextQueue();
  Tetromino next_tetromino();
  const Tetromino& operator[](std::size_t index) const;
};

#endif
