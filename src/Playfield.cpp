#include "Playfield.hpp"
#include "FallingPiece.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <format>
#include <ranges>
#include <utility>

namespace sr = std::ranges;
namespace sv = std::views;
using Ctrlr = Controller;
using HandS = HandlingSettings;
using DrawD = DrawingDetails;

static constexpr std::size_t INITIAL_X_POSITION = (Playfield::WIDTH - 1) / 2;
static constexpr std::size_t INITIAL_Y_POSITION = Playfield::VISIBLE_HEIGHT - 1;
static FallingPiece spawn_tetromino(Tetromino tetromino) {
  return FallingPiece(tetromino, INITIAL_X_POSITION, INITIAL_Y_POSITION);
}

Playfield::Playfield() :
  next_queue(),
  falling_piece(spawn_tetromino(next_queue.next_tetromino())) {
  sr::for_each(grid, [](auto& row) { row.fill(Tetromino::Empty); });
}

void Playfield::restart() {
  auto last_score = this->score;
  *this = Playfield();
  this->score = last_score;
}

bool Playfield::lost() const {
  return has_lost;
}

static bool valid_mino(int x, int y) {
  return x >= 0 && x < Playfield::WIDTH && y >= 0 && y < Playfield::HEIGHT;
}

static bool valid_position(const auto& grid, const FallingPiece& piece) {
  return sr::all_of(piece.map, [&](auto coord) {
    int x = piece.x + coord.x;
    int y = piece.y + coord.y;
    return valid_mino(x, y) && grid[y][x] == Tetromino::Empty;
  });
}

static SpinType is_spin(const FallingPiece& piece, auto&& grid) {
  if (piece.tetromino != Tetromino::T)
    return SpinType::No;

  TetrominoMap corners = {{{-1, -1}, {1, -1}, {1, 1}, {-1, 1}}};
  sr::rotate(corners, corners.begin() + std::to_underlying(piece.orientation));

  auto front_count = sr::count_if(sv::take(corners, 2), [&](auto coord) {
    int x = piece.x + coord.x;
    int y = piece.y + coord.y;
    return !valid_mino(x, y) || grid[y][x] != Tetromino::Empty;
  });

  auto back_count = sr::count_if(sv::drop(corners, 2), [&](auto coord) {
    int x = piece.x + coord.x;
    int y = piece.y + coord.y;
    return !valid_mino(x, y) || grid[y][x] != Tetromino::Empty;
  });

  if (front_count + back_count < 3)
    return SpinType::No;
  if (front_count == 2)
    return SpinType::Proper;
  return SpinType::Mini;
}

void Playfield::solidify_piece() {
  bool topped_out = true;

  for (auto coord : falling_piece.map) {
    int x = coord.x + falling_piece.x;
    int y = coord.y + falling_piece.y;
    grid[y][x] = falling_piece.tetromino;

    if (y >= VISIBLE_HEIGHT)
      topped_out = false;
  }

  SpinType spin_type =
    last_move_rotation ? is_spin(falling_piece, grid) : SpinType::No;

  int cleared_lines = 0;
  for (std::size_t row_idx = 0; row_idx < HEIGHT; row_idx++) {
    if (sr::any_of(grid[row_idx], [](auto m) { return m == Tetromino::Empty; }))
      continue;
    sr::copy_backward(sv::take(grid, row_idx), grid.begin() + row_idx + 1);
    grid.front().fill(Tetromino::Empty);
    cleared_lines++;
  }

  if (cleared_lines == 0) {
    combo = 0;
  } else {
    combo += 1;
    if ((cleared_lines == 4) || (spin_type != SpinType::No))
      b2b += 1;
    else
      b2b = 0;
  }

  score += combo * 50;
  int b2b_factor = (b2b >= 2) ? 3 : 2;
  static constexpr std::array<std::array<int, 5>, 3> score_table = {{
    /* cleared:  0   1    2    3    4  */
    /*NoSpin */ {0, 100, 300, 500, 800},
    /*Mini   */ {100, 200, 400, 0, 0},
    /*Proper */ {400, 800, 1200, 1600, 0},
  }};
  auto spin_index = std::to_underlying(spin_type);
  score += b2b_factor * score_table[spin_index][cleared_lines] / 2;

  message = static_cast<MessageType>(cleared_lines);
  message.spin_type = spin_type;

  bool is_all_clear = sr::all_of(grid, [](const auto& row) {
    return sr::all_of(row, [](Tetromino mino) {
      return mino == Tetromino::Empty;
    });
  });

  if (is_all_clear) {
    message.message = MessageType::AllClear;
    score += 3500 * b2b_factor / 2;
  }

  Tetromino next_tetromino = next_queue.next_tetromino();
  falling_piece = spawn_tetromino(next_tetromino);
  frames_since_drop = 0;
  lock_delay_frames = 0;
  lock_delay_resets = 0;
  can_swap = true;

  bool can_spawn_piece = sr::all_of(falling_piece.map, [&](auto coord) {
    int x = coord.x + falling_piece.x;
    int y = coord.y + falling_piece.y;
    return grid[y][x] == Tetromino::Empty;
  });

  has_lost = topped_out || !can_spawn_piece;
}

void Playfield::handle_swap(const Ctrlr& ctrlr) {
  if (!ctrlr.swap() || !can_swap)
    return;

  Tetromino currentTetromino = falling_piece.tetromino;
  if (holding_piece != Tetromino::Empty)
    falling_piece = spawn_tetromino(holding_piece);
  else
    falling_piece = spawn_tetromino(next_queue.next_tetromino());
  holding_piece = currentTetromino;
  can_swap = false;
  frames_since_drop = 0;
  lock_delay_frames = 0;
  lock_delay_resets = 0;
  last_move_rotation = false;
}

void Playfield::handle_shifts(const Ctrlr& ctrlr, const HandS& hand_set) {
  auto try_shifting = [this](Shift shift) {
    const FallingPiece shiftedPiece = falling_piece.shifted(shift);
    if (valid_position(grid, shiftedPiece)) {
      falling_piece = shiftedPiece;
      lock_delay_frames = 0;
      lock_delay_resets += 1;
      last_move_rotation = false;
    }
  };
  auto try_das = [this](Shift shift) {
    FallingPiece shiftedPiece = falling_piece.shifted(shift);
    while (valid_position(grid, shiftedPiece)) {
      falling_piece = shiftedPiece;
      lock_delay_frames = 0;
      lock_delay_resets += 1;
      last_move_rotation = false;
      shiftedPiece = falling_piece.shifted(shift);
    }
  };

  if (ctrlr.left())
    try_shifting(Shift::Left);
  else if (ctrlr.right())
    try_shifting(Shift::Right);

  if (ctrlr.left_das()) {
    frames_pressed = std::max(0, frames_pressed) + 1;
    if (frames_pressed > hand_set.das)
      try_das(Shift::Left);
  } else if (ctrlr.right_das()) {
    frames_pressed = std::min(0, frames_pressed) - 1;
    if (-frames_pressed > hand_set.das)
      try_das(Shift::Right);
  } else {
    frames_pressed = 0;
  }
}

void Playfield::handle_rotations(const Ctrlr& ctrlr) {
  auto try_rotating = [this](RotationType rotationType) {
    const FallingPiece rotated_piece = falling_piece.rotated(rotationType);
    auto offsets = sv::transform(
      sv::zip(offset_table(falling_piece), offset_table(rotated_piece)),
      [](auto ps) {
        auto [p1, p2] = ps;
        return CoordinatePair(p1.x - p2.x, p1.y - p2.y);
      }
    );
    auto valid_offset = sr::find_if(offsets, [&](auto offset) {
      return valid_position(grid, rotated_piece.translated(offset));
    });
    if (valid_offset != offsets.end()) {
      falling_piece = rotated_piece.translated(*valid_offset);
      lock_delay_frames = 0;
      lock_delay_resets += 1;
      last_move_rotation = true;
    }
  };

  if (ctrlr.clockwise())
    try_rotating(RotationType::Clockwise);
  else if (ctrlr.counter_clockwise())
    try_rotating(RotationType::CounterClockwise);
  else if (ctrlr.one_eighty())
    try_rotating(RotationType::OneEighty);
}

bool Playfield::handle_drops(const Ctrlr& ctrlr, const HandS& hand_set) {
  if (ctrlr.check_hard_drop()) {
    auto fallen = falling_piece.fallen();
    while (valid_position(grid, fallen)) {
      falling_piece = fallen;
      fallen = falling_piece.fallen();
      last_move_rotation = false;
    }
    solidify_piece();
    return true;
  }

  bool soft_fall = ctrlr.soft_drop() && frames_since_drop >= hand_set.soft_drop;
  bool gravity_fall = frames_since_drop >= hand_set.gravity;
  bool is_fall_step = soft_fall || gravity_fall;
  if (is_fall_step)
    frames_since_drop = 0;

  bool can_fall = valid_position(grid, falling_piece.fallen());
  bool can_wait = lock_delay_frames < hand_set.lock_delay_frames;
  bool can_reset = lock_delay_resets < hand_set.lock_delay_resets;
  if (!can_fall && (!can_wait || !can_reset)) {
    solidify_piece();
    return true;
  }

  if (can_fall && is_fall_step) {
    last_move_rotation = false;
    falling_piece.fall();
    lock_delay_frames = 0;
    lock_delay_resets = 0;
  }

  return false;
}

bool Playfield::update(const Ctrlr& ctrlr, const HandS& hand_set) {
  if (has_lost)
    return false;

  handle_swap(ctrlr);

  frames_since_drop += 1;
  lock_delay_frames += 1;
  if (message.timer > 0)
    message.timer -= 1;

  handle_shifts(ctrlr, hand_set);
  handle_rotations(ctrlr);
  return handle_drops(ctrlr, hand_set);
}

namespace {
constexpr Color tetromino_color(Tetromino tetromino) {
  static constexpr std::array<Color, 8> colors = {
    {{49, 199, 239, 255},
     {247, 211, 8, 255},
     {173, 77, 156, 255},
     {239, 32, 41, 255},
     {66, 182, 66, 255},
     {90, 101, 173, 255},
     {239, 121, 33, 255},
     BLANK}
  };
  return colors.at(std::to_underlying(tetromino));
}

inline Rectangle get_block(int i, int j, const DrawD& draw_d) {
  return {
    draw_d.position.x + i * draw_d.block_length,
    draw_d.position.y +
      (j - static_cast<int>(Playfield::VISIBLE_HEIGHT)) * draw_d.block_length,
    draw_d.block_length,
    draw_d.block_length
  };
}

void draw_block_pretty(int i, int j, const DrawD& draw_d, Color fill) {
  if (fill.a == 0)
    return;

  Rectangle rec = get_block(i, j, draw_d);
  DrawRectangleRec(rec, fill);
  DrawRectangle(
    rec.x + draw_d.block_length / 3,
    rec.y + draw_d.block_length / 3,
    rec.width / 3,
    rec.height / 3,
    DrawD::DEFAULT_PRETTY_OUTLINE
  );
  DrawRectangleLinesEx(
    rec, draw_d.block_length / 8, DrawD::DEFAULT_PRETTY_OUTLINE
  );
}

void draw_block_danger(int i, int j, const DrawD& draw_d) {
  Rectangle rec = get_block(i, j, draw_d);
  DrawRectangleLinesEx(rec, draw_d.block_length / 8, {255, 0, 0, 150});
  DrawLineEx(
    {rec.x + rec.width * 0.25f, rec.y + rec.height * 0.25f},
    {rec.x + rec.width * 0.75f, rec.y + rec.height * 0.75f},
    draw_d.block_length * 0.1f,
    RED
  );
  DrawLineEx(
    {rec.x + rec.width * 0.75f, rec.y + rec.height * 0.25f},
    {rec.x + rec.width * 0.25f, rec.y + rec.height * 0.75f},
    draw_d.block_length * 0.1f,
    {255, 0, 0, 150}
  );
}

void draw_piece(
  const TetrominoMap& map,
  Color color,
  int x_offset,
  int y_offset,
  const DrawD& draw_d
) {
  for (auto coord : map) {
    int x = coord.x + x_offset;
    int y = coord.y + y_offset;
    draw_block_pretty(x, y, draw_d, color);
  }
}

void draw_piece_danger(Tetromino tetromino, const DrawD& draw_d) {
  for (auto coord : initial_tetromino_map(tetromino)) {
    int x = coord.x + INITIAL_X_POSITION;
    int y = coord.y + INITIAL_Y_POSITION;
    draw_block_danger(x, y, draw_d);
  }
}

std::pair<const char*, Color> message_info(MessageType message) {
  static constexpr std::array<std::pair<const char*, Color>, 6> info = {
    {{"", BLANK},
     {"SINGLE", {0, 0, 0, 255}},
     {"DOUBLE", {235, 149, 52, 255}},
     {"TRIPLE", {88, 235, 52, 255}},
     {"TETRIS", {52, 164, 236, 255}},
     {"ALL\nCLEAR", {235, 52, 213, 255}}}
  };
  return info.at(std::to_underlying(message));
}
}; // namespace

void Playfield::draw_tetrion(const DrawD& draw_d) const {
  Rectangle tetrion = Rectangle{
    draw_d.position.x,
    draw_d.position.y,
    draw_d.block_length * WIDTH,
    draw_d.block_length * VISIBLE_HEIGHT
  };
  DrawRectangleRec(tetrion, draw_d.TETRION_BACKGROUND_COLOR);
  DrawRectangleLinesEx(
    tetrion, draw_d.block_length / 10, draw_d.GRINDLINE_COLOR
  );

  for (int i = 1; i < WIDTH; ++i) {
    Rectangle rec = get_block(i, VISIBLE_HEIGHT, draw_d);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx(
      {rec.x, rec.y},
      {rec.x, std::floor(rec.y + VISIBLE_HEIGHT * draw_d.block_length)},
      draw_d.block_length / 10,
      draw_d.GRINDLINE_COLOR
    );
  }

  for (int j = 1; j < VISIBLE_HEIGHT; ++j) {
    Rectangle rec = get_block(0, j + VISIBLE_HEIGHT, draw_d);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx(
      {rec.x, rec.y},
      {std::floor(rec.x + draw_d.block_length * WIDTH), rec.y},
      draw_d.block_length / 10,
      draw_d.GRINDLINE_COLOR
    );
  }

  for (int j = 0; j < HEIGHT; ++j)
    for (int i = 0; i < WIDTH; ++i)
      draw_block_pretty(i, j, draw_d, tetromino_color(grid[j][i]));
}

void Playfield::draw_tetrion_pieces(const DrawD& draw_d) const {
  FallingPiece ghost_piece = falling_piece;
  while (valid_position(grid, ghost_piece.fallen()))
    ghost_piece.fall();
  draw_piece(ghost_piece.map, GRAY, ghost_piece.x, ghost_piece.y, draw_d);

  draw_piece(
    falling_piece.map,
    tetromino_color(falling_piece.tetromino),
    falling_piece.x,
    falling_piece.y,
    draw_d
  );

  static constexpr auto X_DANGER_RANGE = sv::iota(WIDTH / 2 - 2, WIDTH / 2 + 2);
  static constexpr auto Y_DANGER_RANGE =
    sv::iota(INITIAL_Y_POSITION, INITIAL_Y_POSITION + 5);
  bool is_in_danger = sr::any_of(X_DANGER_RANGE, [this](auto x) {
    return sr::any_of(Y_DANGER_RANGE, [this, x](auto y) {
      return grid[y][x] != Tetromino::Empty;
    });
  });

  if (is_in_danger)
    draw_piece_danger(next_queue[0], draw_d);
}

void Playfield::draw_next_queue(const DrawD& draw_d) const {
  Rectangle text_rect = get_block(WIDTH + 1, VISIBLE_HEIGHT, draw_d);
  Rectangle background = get_block(WIDTH + 1, VISIBLE_HEIGHT + 2, draw_d);
  background.width = draw_d.block_length * 6;
  background.height = draw_d.block_length * (3 * (NextQueue::NEXT_SIZE) + 1);
  DrawRectangleRec(background, draw_d.PIECES_BACKGROUND_COLOR);
  DrawRectangleLinesEx(
    background, draw_d.block_length / 4, draw_d.PIECE_BOX_COLOR
  );
  DrawText(
    "NEXT", text_rect.x, text_rect.y, draw_d.font_size, draw_d.INFO_TEXT_COLOR
  );
  for (int id = 0; id < NextQueue::NEXT_SIZE; ++id)
    draw_piece(
      initial_tetromino_map(next_queue[id]),
      tetromino_color(next_queue[id]),
      WIDTH + 3,
      3 * (id + 1) + VISIBLE_HEIGHT + 1,
      draw_d
    );
}

void Playfield::draw_hold_piece(const DrawD& draw_d) const {
  Rectangle text_rect = get_block(-7, VISIBLE_HEIGHT, draw_d);
  DrawText(
    "HOLD", text_rect.x, text_rect.y, draw_d.font_size, draw_d.INFO_TEXT_COLOR
  );
  Rectangle background = get_block(-7, VISIBLE_HEIGHT + 2, draw_d);
  background.width = draw_d.block_length * 6;
  background.height = draw_d.block_length * 4;
  DrawRectangleRec(background, draw_d.PIECES_BACKGROUND_COLOR);
  DrawRectangleLinesEx(
    background, draw_d.block_length / 4, draw_d.PIECE_BOX_COLOR
  );

  Color color = can_swap ? tetromino_color(holding_piece) :
                           draw_d.UNAVAILABLE_HOLD_PIECE_COLOR;
  draw_piece(
    initial_tetromino_map(holding_piece), color, -5, 4 + VISIBLE_HEIGHT, draw_d
  );
}

void Playfield::draw_info(const DrawD& draw_d) const {
  if (message.timer > 0) {
    Rectangle text_rect = get_block(draw_d.LEFT_BORDER, HEIGHT - 4, draw_d);
    auto [msg, color] = message_info(message.message);
    unsigned char alpha = (255.0 * message.timer) / LineClearMessage::DURATION;
    color.a = alpha;
    DrawText(msg, text_rect.x, text_rect.y, draw_d.font_size, color);

    if (message.spin_type != SpinType::No) {
      Color spin_color = tetromino_color(Tetromino::T);
      spin_color.a = alpha;
      Rectangle spin_rect = get_block(draw_d.LEFT_BORDER, HEIGHT - 6, draw_d);
      DrawText("TSPIN", spin_rect.x, spin_rect.y, draw_d.font_size, spin_color);
      if (message.spin_type == SpinType::Mini) {
        Rectangle mini_rect = get_block(draw_d.LEFT_BORDER, HEIGHT - 7, draw_d);
        DrawText(
          "MINI", mini_rect.x, mini_rect.y, draw_d.font_size_small, spin_color
        );
      }
    }
  }

  if (combo >= 2) {
    Rectangle combo_rect = get_block(draw_d.LEFT_BORDER, HEIGHT - 10, draw_d);
    DrawText("COMBO ", combo_rect.x, combo_rect.y, draw_d.font_size, BLUE);
    DrawText(
      std::format("{}", combo).c_str(),
      combo_rect.x + MeasureText("COMBO ", draw_d.font_size),
      combo_rect.y,
      draw_d.font_size,
      BLUE
    );
  }

  if (b2b >= 2) {
    Rectangle b2b_rect = get_block(draw_d.LEFT_BORDER, HEIGHT - 12, draw_d);
    DrawText("B2B ", b2b_rect.x, b2b_rect.y, draw_d.font_size, BLUE);
    DrawText(
      std::format("{}", b2b - 1).c_str(),
      b2b_rect.x + MeasureText("B2B ", draw_d.font_size),
      b2b_rect.y,
      draw_d.font_size,
      BLUE
    );
  }

  Rectangle score_rect = get_block(WIDTH + 1, HEIGHT - 2, draw_d);
  DrawText(
    std::format("{:09}", score).c_str(),
    score_rect.x,
    score_rect.y + draw_d.block_length * 0.5,
    draw_d.font_size,
    draw_d.INFO_TEXT_COLOR
  );
}

void Playfield::draw(const DrawD& draw_d) const {
  draw_tetrion(draw_d);
  draw_tetrion_pieces(draw_d);
  draw_next_queue(draw_d);
  draw_hold_piece(draw_d);
  draw_info(draw_d);
}
