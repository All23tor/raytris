#include "FallingPiece.hpp"
#include <random>

class NextQueue {
public:
  static inline std::mt19937 generator = std::mt19937((std::random_device())());
  static constexpr std::size_t NEXT_COMING_SIZE = 5;
  static constexpr std::size_t SIZE_OF_BAG = 7;

private:
  std::vector<Tetromino> queue;

  void pop_back();
  void pushNewBag();

public:
  NextQueue();
  Tetromino getNextTetromino();
  void pushNewBagIfNeeded();
  const Tetromino &operator[](std::size_t index) const;
};
