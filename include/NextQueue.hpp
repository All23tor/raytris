#ifndef NEXT_QUEUE_H
#define NEXT_QUEUE_H

#include "FallingPiece.hpp"
#include <iostream>

class NextQueue {
public:
  static constexpr std::size_t NEXT_COMING_SIZE = 5;
  static constexpr std::size_t SIZE_OF_BAG = 7;
  static constexpr std::size_t MAX_QUEUE_SIZE = NEXT_COMING_SIZE + SIZE_OF_BAG;

private:
  using enum Tetromino;
  std::array<Tetromino, MAX_QUEUE_SIZE> queue = {I, O, T, S, Z, J, L};
  std::size_t queueSize = SIZE_OF_BAG;
  void pushNewBag();

public:
  NextQueue();
  Tetromino getNextTetromino();
  void pushNewBagIfNeeded();
  const Tetromino& operator[](std::size_t index) const;
  friend std::ostream& operator<<(std::ostream&, const NextQueue&);
  friend std::istream& operator>>(std::istream&, NextQueue&);
};

#endif
