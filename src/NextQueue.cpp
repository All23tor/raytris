#include "NextQueue.hpp"
#include "FallingPiece.hpp"
#include <algorithm>
#include <random>

namespace {
std::mt19937 generator = std::mt19937((std::random_device())());
};

NextQueue::NextQueue() {
  std::shuffle(queue.begin(), queue.begin() + SizeOfBag, generator);
}

void NextQueue::pushNewBag() {
  using enum Tetromino;
  std::array<Tetromino, SizeOfBag> newBag = {I, O, T, S, Z, J, L};
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

std::ostream& operator<<(std::ostream& out, const NextQueue& nq) {
  for (const auto& tetromino : nq.queue) {
    out << static_cast<int>(tetromino) << ' ';
  }
  return out << '\n' << nq.queueSize << '\n';
}

std::istream& operator>>(std::istream& in, NextQueue& nq) {
  for (auto& tetromino : nq.queue) {
    int input_tetromino;
    in >> input_tetromino;
    tetromino = static_cast<Tetromino>(input_tetromino);
  }
  return in >> nq.queueSize;
}
