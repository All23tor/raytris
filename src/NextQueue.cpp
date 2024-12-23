#include "NextQueue.hpp"
#include <algorithm>

void NextQueue::pushNewBag() {
  using enum Tetromino;
  queue.insert(queue.begin(), { I, O, T, S, Z, J, L });
  std::shuffle(queue.begin(), queue.begin() + SizeOfBag, generator);
}

NextQueue::NextQueue() {
  pushNewBag();
}

Tetromino NextQueue::getNextTetromino() {
  Tetromino tmp = queue.back();
  queue.pop_back();
  return tmp;
}

void NextQueue::pushNewBagIfNeeded() {
  if (queue.size() < SizeOfBag) {
    pushNewBag();
  }
}

const Tetromino& NextQueue::operator[](std::size_t index) const {
  return queue[queue.size() - 1 - index];
}
