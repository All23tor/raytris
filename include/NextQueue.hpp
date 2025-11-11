#ifndef NEXT_QUEUE_H
#define NEXT_QUEUE_H

#include "FallingPiece.hpp"

class NextQueue {
public:
  static constexpr std::size_t NEXT_COMING_SIZE = 5;
  static constexpr std::size_t SIZE_OF_BAG = 7;
  static constexpr std::size_t MAX_QUEUE_SIZE = NEXT_COMING_SIZE + SIZE_OF_BAG;

private:
  using enum Tetromino;
  std::array<Tetromino, MAX_QUEUE_SIZE> queue = {I, O, T, S, Z, J, L};
  std::size_t queue_size = SIZE_OF_BAG;

public:
  NextQueue();
  Tetromino next_tetromino();
  void push_new_bag_if_needed();
  const Tetromino& operator[](std::size_t index) const;
};

#endif
