#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "Controller.hpp"
#include "DrawingDetails.hpp"
#include "HandlingSettings.hpp"
#include "NextQueue.hpp"

enum class MessageType : unsigned char {
  Empty,
  Single,
  Double,
  Triple,
  Tetris,
  AllClear,
};

enum class SpinType {
  No,
  Mini,
  Proper,
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

  Playfield();
  bool lost() const;
  bool update(const Controller&, const HandlingSettings&);
  void draw(const DrawingDetails&) const;
  void restart();

private:
  std::array<std::array<Tetromino, WIDTH>, HEIGHT> grid;
  NextQueue next_queue;
  FallingPiece falling_piece;
  Tetromino holding_piece = Tetromino::Empty;
  bool can_swap = true;
  unsigned int frames_since_drop = 0;
  unsigned int lock_delay_frames = 0;
  unsigned int lock_delay_resets = 0;
  int frames_pressed = 0;
  unsigned int combo = 0;
  bool has_lost = false;
  unsigned long score = 0;
  unsigned int b2b = 0;
  bool last_move_rotation = false;
  LineClearMessage message;

  void handle_swap(const Controller&);
  void handle_shifts(const Controller&, const HandlingSettings&);
  void handle_rotations(const Controller&);
  bool handle_drops(const Controller&, const HandlingSettings&);
  void solidify_piece();

  void draw_tetrion(const DrawingDetails&) const;
  void draw_tetrion_pieces(const DrawingDetails&) const;
  void draw_next_queue(const DrawingDetails&) const;
  void draw_hold_piece(const DrawingDetails&) const;
  void draw_info(const DrawingDetails&) const;
};

#endif
