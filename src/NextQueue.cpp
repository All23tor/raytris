#include "NextQueue.hpp"
#include "FallingPiece.hpp"
#include <algorithm>
#include <random>

using enum Tetromino;
static std::mt19937 generator = std::mt19937((std::random_device())());
static constexpr std::array<Tetromino, NextQueue::SIZE_OF_BAG> new_bag = {
  I, O, T, S, Z, J, L
};

NextQueue::NextQueue() : queue_size{SIZE_OF_BAG} {
  std::ranges::copy(new_bag, queue.begin());
  std::shuffle(queue.begin(), queue.begin() + SIZE_OF_BAG, generator);
}

Tetromino NextQueue::next_tetromino() {
  Tetromino next = queue[--queue_size];
  if (queue_size <= NEXT_SIZE) {
    for (std::size_t index = queue_size; index >= 1; index--)
      queue[index + SIZE_OF_BAG - 1] = queue[index - 1];
    for (std::size_t index = 0; index < SIZE_OF_BAG; index++)
      queue[index] = new_bag[index];
    queue_size += SIZE_OF_BAG;
    std::shuffle(queue.begin(), queue.begin() + SIZE_OF_BAG, generator);
  }
  return next;
}

const Tetromino& NextQueue::operator[](std::size_t index) const {
  return queue[queue_size - 1 - index];
}
