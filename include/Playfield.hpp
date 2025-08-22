#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "Controller.hpp"
#include "DrawingDetails.hpp"
#include "HandlingSettings.hpp"
#include "NextQueue.hpp"
#include <iostream>

enum class MessageType : unsigned char {
  Single,
  Double,
  Triple,
  Tetris,
  AllClear,
  Empty
};

enum class SpinType {
  No,
  Proper,
  Mini
};

struct LineClearMessage {
  static constexpr unsigned char DURATION = 180;

  MessageType message;
  unsigned char timer;
  SpinType spinType;

  LineClearMessage() :
    message(MessageType::Empty),
    timer(0),
    spinType(SpinType::No) {}
  LineClearMessage(MessageType type) :
    message(type),
    timer(DURATION),
    spinType(SpinType::No) {}
};

class Playfield {
public:
  static constexpr std::size_t WIDTH = 10;
  static constexpr std::size_t HEIGHT = 40;
  static constexpr std::size_t VISIBLE_HEIGHT = 20;

private:
  // Game abstractions
  std::array<std::array<Tetromino, WIDTH>, HEIGHT> grid;
  FallingPiece fallingPiece;
  Tetromino holdingPiece = Tetromino::Empty;
  NextQueue nextQueue;
  // State
  bool canSwap = true;
  unsigned int framesSinceLastFall = 0;
  unsigned int lockDelayFrames = 0;
  unsigned int lockDelayMoves = 0;
  int signedFramesPressed = 0;
  unsigned int combo = 0;
  bool hasLost = false;
  unsigned long score = 0;
  unsigned int b2b = 0;
  bool wasLastMoveRotation = false;
  LineClearMessage message;

public:
  Playfield();
  bool lost() const;
  bool update(const Controller&, const HandlingSettings&);
  void draw(const DrawingDetails&) const;
  void restart();
  friend std::ostream& operator<<(std::ostream&, const Playfield&);
  friend std::istream& operator>>(std::istream&, Playfield&);

private:
  bool isValidPosition(const FallingPiece&) const;
  bool tryShifting(Shift);
  bool tryRotating(RotationType);
  SpinType isSpin() const;
  void solidify();
  void updateScore(std::size_t, SpinType);
  bool isAllClear() const;
  void clearLines();
  bool isInDanger() const;

  void swapTetromino();
  void replaceNextPiece();
  void updateTimers();
  void handleSpecialInput(const Controller&);
  void handleShiftInput(const Controller&, const HandlingSettings&);
  void handleRotationInput(const Controller&);
  bool handleDropInput(const Controller&, const HandlingSettings&);
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
