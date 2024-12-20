#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "NextQueue.hpp"
#include "Controller.hpp"

enum class MessageType : unsigned char {
  SINGLE,
  DOUBLE,
  TRIPLE,
  TETRIS,
  ALLCLEAR,
  EMPTY
};

enum class SpinType { No, Proper, Mini };

struct LineClearMessage {
  static constexpr unsigned char DURATION = 180;

  MessageType message;
  unsigned char timer;
  SpinType spinType;

  LineClearMessage() : message(MessageType::EMPTY), timer(0), spinType(SpinType::No) {}
  LineClearMessage(MessageType type) : message(type), timer(DURATION), spinType(SpinType::No) {}
};

class Playfield {
  friend class Game;

public:
  static constexpr std::size_t WIDTH = 10;
  static constexpr std::size_t HEIGHT = 40;
  static constexpr std::size_t VISIBLE_HEIGHT = 20;
  static constexpr std::size_t INITIAL_HORIZONTAL_POSITION = (WIDTH - 1) / 2;
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
  Tetromino holdingPiece = Tetromino::Empty;
  NextQueue nextQueue;
  // State
  bool canSwap = true;
  unsigned char framesSinceLastFall = 0;
  unsigned char lockDelayFrames = 0;
  unsigned char lockDelayMoves = 0;
  int signedFramesPressed = 0;
  unsigned char combo = 0;
  bool hasLost = false;
  unsigned long score = 0;
  unsigned char b2b = 0;
  bool wasLastMoveRotation = false;
  LineClearMessage message;

public:
  Playfield();
  bool update(const Controller&);
  void restart();

private:
  bool isValidPosition(const FallingPiece&) const;
  bool tryShifting(Shift);
  bool tryRotating(RotationType);
  SpinType isSpin() const;
  void solidify();
  void clearRows(const std::vector<std::size_t>&);
  void updateScore(std::size_t, SpinType);
  bool isAllClear() const;
  void clearLines();

  void swapTetromino();
  void replaceNextPiece();
  void updateTimers();
  void handleSpecialInput(const Controller&);
  void handleShiftInput(const Controller&);
  void handleRotationInput(const Controller&);
  bool handleDropInput(const Controller&);
  FallingPiece getGhostPiece() const;
};

#endif
