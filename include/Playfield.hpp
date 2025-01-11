#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "NextQueue.hpp"
#include "Controller.hpp"
#include "DrawingDetails.hpp"

enum class MessageType : unsigned char {
  Single,
  Double,
  Triple,
  Tetris,
  AllClear,
  Empty
};

enum class SpinType { No, Proper, Mini };

struct LineClearMessage {
  static constexpr unsigned char Duration = 180;

  MessageType message;
  unsigned char timer;
  SpinType spinType;

  LineClearMessage() : message(MessageType::Empty), timer(0), spinType(SpinType::No) {}
  LineClearMessage(MessageType type) : message(type), timer(Duration), spinType(SpinType::No) {}
};

class Playfield {
public:
  static constexpr std::size_t Width = 10;
  static constexpr std::size_t Height = 40;
  static constexpr std::size_t VisibleHeight = 20;
  static constexpr std::size_t InitialHorizontalPosition = (Width - 1) / 2;
  static constexpr std::size_t InitialVerticalPosition = VisibleHeight - 1;
  static constexpr unsigned GravityFrames = 20;
  static constexpr unsigned SoftDropFrames = 1;
  static constexpr unsigned char MaxLockDelayFrames = 30;
  static constexpr unsigned char MaxLockDelayResets = 15;
  static constexpr unsigned char Das = 7;

private:
  // Game abstractions
  std::array<std::array<Tetromino, Width>, Height> grid;
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
  bool lost() const;
  bool update(const Controller&);
  void draw(const DrawingDetails&) const;
  void restart();

private:
  bool isValidPosition(const FallingPiece&) const;
  bool tryShifting(Shift);
  bool tryRotating(RotationType);
  SpinType isSpin() const;
  void solidify();
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

  void DrawTetrion(const DrawingDetails&) const;
  void DrawPieces(const DrawingDetails&) const;
  void DrawNextComingPieces(const DrawingDetails&) const;
  void DrawHoldPiece(const DrawingDetails&) const;
  void DrawLineClearMessage(const DrawingDetails&) const;
  void DrawCombo(const DrawingDetails&) const;
  void DrawBackToBack(const DrawingDetails&) const;
  void DrawScore(const DrawingDetails&) const;
};

#endif
