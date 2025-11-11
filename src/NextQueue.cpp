#include "NextQueue.hpp"
#include "FallingPiece.hpp"
#include <algorithm>
#include <random>

static std::mt19937 generator = std::mt19937((std::random_device())());

NextQueue::NextQueue() {
  std::shuffle(queue.begin(), queue.begin() + SIZE_OF_BAG, generator);
}

Tetromino NextQueue::next_tetromino() {
  return queue[--queue_size];
}

void NextQueue::push_new_bag_if_needed() {
  if (queue_size > NEXT_COMING_SIZE)
    return;

  using enum Tetromino;
  std::array<Tetromino, SIZE_OF_BAG> newBag = {I, O, T, S, Z, J, L};
  std::shuffle(newBag.begin(), newBag.end(), generator);
  for (std::size_t index = queue_size; index >= 1; index--)
    queue[index + SIZE_OF_BAG - 1] = queue[index - 1];

  for (std::size_t index = 0; index < SIZE_OF_BAG; index++)
    queue[index] = newBag[index];

  queue_size += SIZE_OF_BAG;
}

const Tetromino& NextQueue::operator[](std::size_t index) const {
  return queue[queue_size - 1 - index];
}
