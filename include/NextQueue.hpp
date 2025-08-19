#ifndef NEXT_QUEUE_H
#define NEXT_QUEUE_H

#include "FallingPiece.hpp"
#include <iostream>

class NextQueue {
public:
  static constexpr std::size_t NextComingSize = 5;
  static constexpr std::size_t SizeOfBag = 7;
  static constexpr std::size_t MaxQueueSize = NextComingSize + SizeOfBag;

private:
  using enum Tetromino;
  std::array<Tetromino, MaxQueueSize> queue = {I, O, T, S, Z, J, L};
  std::size_t queueSize = SizeOfBag;
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
