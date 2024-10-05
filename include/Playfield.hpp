#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "NextQueue.hpp"

enum class Shift : unsigned char { LEFT, RIGHT };
enum class RotationType : unsigned char {
  Clockwise,
  CounterClockwise,
  OneEighty
};

enum class MessageType : unsigned char {
  SINGLE,
  DOUBLE,
  TRIPLE,
  TETRIS,
  ALLCLEAR,
  EMPTY_MESSAGE
};

struct LineClearMessage {
  static constexpr unsigned char DURATION = 180;

  MessageType message;
  unsigned char timer;

  LineClearMessage() : message(MessageType::EMPTY_MESSAGE), timer(0) {}
  LineClearMessage(MessageType type) : message(type), timer(DURATION) {}
};

class Playfield {
  friend class Game;

public:
  static constexpr std::size_t WIDTH = 10;
  static constexpr std::size_t HEIGHT = 40;
  static constexpr std::size_t VISIBLE_HEIGHT = 20;
  static constexpr std::size_t INITIAL_HORIAZONTAL_POSITION = (WIDTH - 1) / 2;
  static constexpr std::size_t INITIAL_VERTICAL_POSITION = VISIBLE_HEIGHT - 1;
  static constexpr unsigned GRAVITY_FRAMES = 20;
  static constexpr unsigned SOFT_DROP_FRAMES = 1;
  static constexpr unsigned char MAX_LOCK_DELAY_FRAMES = 30;
  static constexpr unsigned char MAX_LOCK_DELAY_RESETS = 15;
  static constexpr unsigned char DAS = 7;

private:
  // Game abstractions
  std::array<std::array<Tetromino, WIDTH>, HEIGHT> grid;
  FallingPiece fallingPiece;
  Tetromino holdingPiece = Tetromino::EMPTY;
  NextQueue next_queue;
  // State
  bool canSwap = false;
  unsigned char frames_since_last_fall = 0;
  unsigned char lock_delay_frames = 0;
  unsigned char lock_delay_moves = 0;
  int signed_frames_pressed = 0;
  unsigned char combo = 0;
  bool hasLost = false;
  unsigned long score = 0;
  unsigned char b2b = 0;
  LineClearMessage message;

public:
  Playfield();
  bool update();
  void restart();

private:
  bool checkFallingCollisions();
  bool shiftFallingPiece(Shift);
  void checkRotationCollision(RotationType);
  void solidifyFallingPiece();
  void clearRows(std::vector<int> &, char);
  bool isAllClear();
  void clearLines();
  void swapTetromino();
  void updateTimers();
  FallingPiece getGhostPiece() const;
};

#endif
