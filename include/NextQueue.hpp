#ifndef NEXT_QUEUE_H
#define NEXT_QUEUE_H

#include "FallingPiece.hpp"
#include <random>

class NextQueue {
public:
  static inline std::mt19937 generator = std::mt19937((std::random_device())());
  static constexpr std::size_t NextComingSize = 5;
  static constexpr std::size_t SizeOfBag = 7;

private:
  std::vector<Tetromino> queue;

  void popBack();
  void pushNewBag();

public:
  NextQueue();
  Tetromino getNextTetromino();
  void pushNewBagIfNeeded();
  const Tetromino &operator[](std::size_t index) const;
};

#endif
