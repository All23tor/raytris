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
  SpinType spin_type;

  LineClearMessage() :
    message(MessageType::Empty),
    timer(0),
    spin_type(SpinType::No) {}
  LineClearMessage(MessageType type) :
    message(type),
    timer(DURATION),
    spin_type(SpinType::No) {}
};

class Playfield {
public:
  static constexpr std::size_t WIDTH = 10;
  static constexpr std::size_t HEIGHT = 40;
  static constexpr std::size_t VISIBLE_HEIGHT = 20;

private:
  // Game abstractions
  std::array<std::array<Tetromino, WIDTH>, HEIGHT> grid;
  FallingPiece falling_piece;
  Tetromino holding_piece = Tetromino::Empty;
  NextQueue next_queue;
  // State
  bool can_swap = true;
  unsigned int frames_since_last_fall = 0;
  unsigned int lock_delay_frames = 0;
  unsigned int lock_delay_moves = 0;
  int frames_pressed = 0;
  unsigned int combo = 0;
  bool has_lost = false;
  unsigned long score = 0;
  unsigned int b2b = 0;
  bool last_move_rotation = false;
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
  bool tryShifting(Shift);
  bool tryRotating(RotationType);
  SpinType isSpin() const;
  void solidify();
  void updateScore(std::size_t, SpinType);
  void clearLines();
  bool isInDanger() const;

  void swapTetromino();
  void replaceNextPiece();
  void updateTimers();
  void handleSpecialInput(const Controller&);
  void handleShiftInput(const Controller&, const HandlingSettings&);
  void handleRotationInput(const Controller&);
  bool handleDropInput(const Controller&, const HandlingSettings&);
  FallingPiece get_ghost_piece() const;

  void draw_tetrion(const DrawingDetails&) const;
  void draw_pieces(const DrawingDetails&) const;
  void draw_next_pieces(const DrawingDetails&) const;
  void draw_hold_piece(const DrawingDetails&) const;
  void draw_line_clear_message(const DrawingDetails&) const;
  void draw_combo(const DrawingDetails&) const;
  void draw_b2b(const DrawingDetails&) const;
  void draw_score(const DrawingDetails&) const;
};

#endif
