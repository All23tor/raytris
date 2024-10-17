#include "NextQueue.hpp"
#include <algorithm>

void NextQueue::popBack() { queue.pop_back(); }

void NextQueue::pushNewBag() {
  queue.insert(queue.begin(),
               {Tetromino::I, Tetromino::O, Tetromino::T, Tetromino::S,
                Tetromino::Z, Tetromino::J, Tetromino::L});
  std::shuffle(queue.begin(), queue.begin() + SIZE_OF_BAG, generator);
}

NextQueue::NextQueue() { pushNewBag(); }

Tetromino NextQueue::getNextTetromino() {
  Tetromino tmp = queue.back();
  queue.pop_back();
  return tmp;
}

void NextQueue::pushNewBagIfNeeded() {
  if (queue.size() < SIZE_OF_BAG) {
    pushNewBag();
  }
}

const Tetromino &NextQueue::operator[](std::size_t index) const {
  return queue[queue.size() - 1 - index];
}
