#include "NextQueue.hpp"
#include "FallingPiece.hpp"
#include <algorithm>
#include <random>

namespace {
  std::mt19937 generator = std::mt19937((std::random_device())());
};

void NextQueue::pushNewBag() {
  using enum Tetromino;
  std::array<Tetromino, SizeOfBag> newBag =  { I, O, T, S, Z, J, L }; 
  std::shuffle(newBag.begin(), newBag.end(), generator);
  for (std::size_t index = queueSize; index >= 1; index--) {
    queue[index + SizeOfBag - 1] = queue[index - 1];
  }
  for (std::size_t index = 0; index < SizeOfBag; index++) {
    queue[index] = newBag[index];
  }
  queueSize += SizeOfBag;
}

Tetromino NextQueue::getNextTetromino() {
  queueSize--;
  return queue[queueSize];
}

void NextQueue::pushNewBagIfNeeded() {
  if (queueSize > NextComingSize) {
    return;
  }

  pushNewBag();
}

const Tetromino& NextQueue::operator[](std::size_t index) const {
  return queue[queueSize - 1 - index];
}
