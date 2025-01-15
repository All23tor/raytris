#ifndef NEXT_QUEUE_H
#define NEXT_QUEUE_H

#include "FallingPiece.hpp"
#include <vector>

class NextQueue {
public:
  static constexpr std::size_t NextComingSize = 5;
  static constexpr std::size_t SizeOfBag = 7;

private:
  std::vector<Tetromino> queue;
  void pushNewBag();

public:
  NextQueue();
  Tetromino getNextTetromino();
  void pushNewBagIfNeeded();
  const Tetromino &operator[](std::size_t index) const;
};

#endif
