#ifndef NEXT_QUEUE_H
#define NEXT_QUEUE_H

#include "FallingPiece.hpp"

class NextQueue {
public:
  static constexpr std::size_t NextComingSize = 5;
  static constexpr std::size_t SizeOfBag = 7;

private:
  using enum Tetromino;
  std::array<Tetromino, NextComingSize + SizeOfBag> queue = { I, O, T, S, Z, J, L };
  std::size_t queueSize = SizeOfBag;
  void pushNewBag();

public:
  Tetromino getNextTetromino();
  void pushNewBagIfNeeded();
  const Tetromino &operator[](std::size_t index) const;
};

#endif
