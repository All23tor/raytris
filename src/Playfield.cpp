#include "Playfield.hpp"
#include "Controller.hpp"
#include "DrawingDetails.hpp"
#include "FallingPiece.hpp"
#include "HandlingSettings.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <format>
#include <ranges>
#include <utility>

namespace sr = std::ranges;
namespace sv = std::views;

static constexpr std::size_t INITIAL_HORIZONTAL_POSITION =
  (Playfield::WIDTH - 1) / 2;
static constexpr std::size_t INITIAL_VERTICAL_POSITION =
  Playfield::VISIBLE_HEIGHT - 1;
static constexpr FallingPiece spawn_tetromino(Tetromino tetromino) {
  return FallingPiece(
    tetromino, INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION
  );
}

Playfield::Playfield() :
  next_queue(),
  falling_piece(
    next_queue.next_tetromino(),
    INITIAL_HORIZONTAL_POSITION,
    INITIAL_VERTICAL_POSITION
  ) {
  std::array<Tetromino, WIDTH> emptyRow;
  emptyRow.fill(Tetromino::Empty);
  grid.fill(emptyRow);
}

void Playfield::restart() {
  auto lastScore = this->score;
  *this = Playfield();
  this->score = lastScore;
}

bool Playfield::lost() const {
  return has_lost;
}

static bool is_valid_position(const auto& grid, const FallingPiece& piece) {
  return sr::all_of(piece.tetromino_map, [&](auto&& coord) {
    int x = coord.x + piece.x_position;
    int y = coord.y + piece.y_position;
    return x >= 0 && x < Playfield::WIDTH && y >= 0 && y < Playfield::HEIGHT &&
      grid[y][x] == Tetromino::Empty;
  });
}

SpinType is_spin(const FallingPiece& piece, auto&& grid) {
  if (piece.tetromino != Tetromino::T)
    return SpinType::No;

  auto fp_or_idx = std::to_underlying(piece.orientation);
  TetrominoMap corners = {{{-1, -1}, {1, -1}, {1, 1}, {-1, 1}}};
  sr::rotate(corners, corners.begin() + fp_or_idx);

  auto front_count = sr::count_if(sv::take(corners, 2), [&](auto&& coord) {
    int i = piece.x_position + coord.x;
    int j = piece.y_position + coord.y;
    return i < 0 || i >= Playfield::WIDTH || j < 0 || j >= Playfield::HEIGHT ||
      grid[j][i] != Tetromino::Empty;
  });

  auto back_count = sr::count_if(sv::drop(corners, 2), [&](auto&& coord) {
    int i = piece.x_position + coord.x;
    int j = piece.y_position + coord.y;
    return i < 0 || i >= Playfield::WIDTH || j < 0 || j >= Playfield::HEIGHT ||
      grid[j][i] != Tetromino::Empty;
  });

  if (front_count + back_count < 3)
    return SpinType::No;
  if (front_count == 2)
    return SpinType::Proper;
  return SpinType::Mini;
}

void Playfield::solidify_piece() {
  bool anyMinoSolidifedAboveVisibleHeight = false;

  for (auto&& coord : falling_piece.tetromino_map) {
    int x = coord.x + falling_piece.x_position;
    int y = coord.y + falling_piece.y_position;
    grid[y][x] = falling_piece.tetromino;

    if (y >= VISIBLE_HEIGHT)
      anyMinoSolidifedAboveVisibleHeight = true;
  }

  std::array<std::size_t, 4> rows_to_clear;
  std::size_t cleared_lines = 0;
  for (std::size_t j = 0; j < HEIGHT && cleared_lines != 4; ++j) {
    bool isRowFull = std::all_of(grid[j].begin(), grid[j].end(), [](auto mino) {
      return mino != Tetromino::Empty;
    });

    if (isRowFull)
      rows_to_clear[cleared_lines++] = j;
  }

  SpinType spin_type =
    last_move_rotation ? is_spin(falling_piece, grid) : SpinType::No;
  for (std::size_t index = 0; index < cleared_lines; index++) {
    std::size_t row_idx = rows_to_clear[index];
    sr::copy_backward(sv::take(grid, row_idx), grid.begin() + row_idx + 1);
    grid[0].fill(Tetromino::Empty);
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

  update_score(cleared_lines, spin_type);

  Tetromino next_tetromino = next_queue.next_tetromino();
  falling_piece = spawn_tetromino(next_tetromino);
  frames_since_last_fall = 0;
  lock_delay_frames = 0;
  lock_delay_moves = 0;
  can_swap = true;

  bool canSpawnPiece =
    sr::all_of(falling_piece.tetromino_map, [&](auto&& coord) {
      int i = coord.x + falling_piece.x_position;
      int j = coord.y + falling_piece.y_position;
      return grid[j][i] == Tetromino::Empty;
    });

  has_lost = !anyMinoSolidifedAboveVisibleHeight || !canSpawnPiece;
}

void Playfield::update_score(std::size_t cleared_lines, SpinType spin_type) {
  score += combo * 50;
  float b2bFactor = (b2b >= 2) ? 1.5f : 1.0f;

  if (spin_type == SpinType::Proper) {
    if (cleared_lines == 1) {
      message = MessageType::Single;
      score += 800 * b2bFactor;
    } else if (cleared_lines == 2) {
      message = MessageType::Double;
      score += 1200 * b2bFactor;
    } else if (cleared_lines == 3) {
      message = MessageType::Triple;
      score += 1600 * b2bFactor;
    } else {
      message = MessageType::Empty;
      score += 400 * b2bFactor;
    }
    message.spin_type = SpinType::Proper;
  } else if (spin_type == SpinType::Mini) {
    if (cleared_lines == 1) {
      message = MessageType::Single;
      score += 200 * b2bFactor;
    } else if (cleared_lines == 2) {
      message = MessageType::Double;
      score += 400 * b2bFactor;
    } else {
      message = MessageType::Empty;
      score += 100 * b2bFactor;
    }
    message.spin_type = SpinType::Mini;
  } else {
    if (cleared_lines == 1) {
      message = MessageType::Single;
      score += 100 * b2bFactor;
    } else if (cleared_lines == 2) {
      message = MessageType::Double;
      score += 300 * b2bFactor;
    } else if (cleared_lines == 3) {
      message = MessageType::Triple;
      score += 500 * b2bFactor;
    } else if (cleared_lines == 4) {
      message = MessageType::Tetris;
      score += 800 * b2bFactor;
    }
  }

  bool is_all_clear = sr::all_of(grid, [](const auto& row) {
    return sr::all_of(row, [](Tetromino mino) {
      return mino == Tetromino::Empty;
    });
  });

  if (is_all_clear) {
    message = MessageType::AllClear;
    score += 3500 * b2bFactor;
  }
}

void Playfield::handle_swap(const Controller& ctrl) {
  if (!ctrl.swap() || !can_swap)
    return;

  Tetromino currentTetromino = falling_piece.tetromino;
  if (holding_piece != Tetromino::Empty)
    falling_piece = spawn_tetromino(holding_piece);
  else
    falling_piece = spawn_tetromino(next_queue.next_tetromino());
  holding_piece = currentTetromino;
  can_swap = false;
  frames_since_last_fall = 0;
  lock_delay_frames = 0;
  lock_delay_moves = 0;
  last_move_rotation = false;
}

void Playfield::handle_shifts(
  const Controller& ctrl, const HandlingSettings& settings
) {
  auto try_shifting = [this](Shift shift) {
    const FallingPiece shiftedPiece = falling_piece.shifted(shift);
    if (is_valid_position(grid, shiftedPiece)) {
      falling_piece = shiftedPiece;
      lock_delay_frames = 0;
      lock_delay_moves += 1;
      last_move_rotation = false;
    }
  };
  auto try_das = [this](Shift shift) {
    FallingPiece shiftedPiece = falling_piece.shifted(shift);
    while (is_valid_position(grid, shiftedPiece)) {
      falling_piece = shiftedPiece;
      lock_delay_frames = 0;
      lock_delay_moves += 1;
      last_move_rotation = false;
      shiftedPiece = falling_piece.shifted(shift);
    }
  };

  if (ctrl.left())
    try_shifting(Shift::Left);
  else if (ctrl.right())
    try_shifting(Shift::Right);

  if (ctrl.left_das()) {
    frames_pressed = std::max(0, frames_pressed) + 1;
    if (frames_pressed > settings.das)
      try_das(Shift::Left);
  } else if (ctrl.right_das()) {
    frames_pressed = std::min(0, frames_pressed) - 1;
    if (-frames_pressed > settings.das)
      try_das(Shift::Right);
  } else {
    frames_pressed = 0;
  }
}

void Playfield::handle_rotations(const Controller& ctrl) {
  auto try_rotating = [this](RotationType rotationType) {
    const FallingPiece rotated_piece = falling_piece.rotated(rotationType);
    auto offsets = sv::transform(
      sv::zip(get_offset_table(falling_piece), get_offset_table(rotated_piece)),
      [](auto ps) {
        auto [p1, p2] = ps;
        return CoordinatePair(p1.x - p2.x, p1.y - p2.y);
      }
    );

    for (auto&& offset : offsets) {
      const auto translated_piece = rotated_piece.translated(offset);
      if (is_valid_position(grid, translated_piece)) {
        falling_piece = translated_piece;
        lock_delay_frames = 0;
        lock_delay_moves += 1;
        last_move_rotation = true;
        return;
      }
    }
  };

  if (ctrl.clockwise())
    try_rotating(RotationType::Clockwise);
  if (ctrl.counter_clockwise())
    try_rotating(RotationType::CounterClockwise);
  if (ctrl.one_eighty())
    try_rotating(RotationType::OneEighty);
}

bool Playfield::handle_drops(
  const Controller& ctrl, const HandlingSettings& settings
) {
  if (ctrl.check_hard_drop()) {
    while (is_valid_position(grid, falling_piece.fallen())) {
      falling_piece.fall();
      last_move_rotation = false;
    }
    solidify_piece();
    return true;
  }

  bool is_fall_step = false;
  if (ctrl.soft_drop()) {
    // Soft Drop
    if (frames_since_last_fall >= settings.softDropFrames) {
      frames_since_last_fall = 0;
      is_fall_step = true;
    }
  } else if (frames_since_last_fall >= settings.gravityFrames) {
    // Gravity
    frames_since_last_fall = 0;
    is_fall_step = true;
  }

  if (is_valid_position(grid, falling_piece.fallen())) {
    // Is not touching ground
    if (is_fall_step) {
      last_move_rotation = false;
      falling_piece.fall();
      lock_delay_frames = 0;
      lock_delay_moves = 0;
    }

    return false;
  }
  // Is touching ground
  if (lock_delay_frames > settings.maxLockDelayFrames ||
      lock_delay_moves > settings.maxLockDelayResets) {
    solidify_piece();
    return true;
  }
  return false;
}

bool Playfield::update(const Controller& ctrl, const HandlingSettings& hand_s) {
  if (has_lost)
    return false;

  handle_swap(ctrl);

  frames_since_last_fall += 1;
  lock_delay_frames += 1;
  if (message.timer > 0)
    message.timer -= 1;

  next_queue.push_new_bag_if_needed();

  handle_shifts(ctrl, hand_s);
  handle_rotations(ctrl);
  return handle_drops(ctrl, hand_s);
}

namespace {
constexpr Color tetromino_color(Tetromino tetromino) {
  switch (tetromino) {
  case Tetromino::I:
    return (Color){49, 199, 239, 255};
  case Tetromino::O:
    return (Color){247, 211, 8, 255};
  case Tetromino::T:
    return (Color){173, 77, 156, 255};
  case Tetromino::S:
    return (Color){66, 182, 66, 255};
  case Tetromino::Z:
    return (Color){239, 32, 41, 255};
  case Tetromino::J:
    return (Color){90, 101, 173, 255};
  case Tetromino::L:
    return (Color){239, 121, 33, 255};
  default:
    return BLANK;
  }
}

inline Rectangle get_block(int i, int j, const DrawingDetails& draw_d) {
  return {
    draw_d.position.x + i * draw_d.block_length,
    draw_d.position.y +
      (j - static_cast<int>(Playfield::VISIBLE_HEIGHT)) * draw_d.block_length,
    draw_d.block_length,
    draw_d.block_length
  };
}

void draw_block_pretty(
  int i, int j, const DrawingDetails& details, Color fill
) {
  if (fill.a == 0)
    return;

  Rectangle rec = get_block(i, j, details);
  DrawRectangleRec(rec, fill);
  DrawRectangle(
    rec.x + details.block_length / 3,
    rec.y + details.block_length / 3,
    rec.width / 3,
    rec.height / 3,
    DrawingDetails::DEFAULT_PRETTY_OUTLINE
  );
  DrawRectangleLinesEx(
    rec, details.block_length / 8, DrawingDetails::DEFAULT_PRETTY_OUTLINE
  );
}

void draw_block_danger(int i, int j, const DrawingDetails& details) {
  Rectangle rec = get_block(i, j, details);
  DrawRectangleLinesEx(rec, details.block_length / 8, {255, 0, 0, 150});
  DrawLineEx(
    {rec.x + rec.width * 0.25f, rec.y + rec.height * 0.25f},
    {rec.x + rec.width * 0.75f, rec.y + rec.height * 0.75f},
    details.block_length * 0.1f,
    RED
  );
  DrawLineEx(
    {rec.x + rec.width * 0.75f, rec.y + rec.height * 0.25f},
    {rec.x + rec.width * 0.25f, rec.y + rec.height * 0.75f},
    details.block_length * 0.1f,
    {255, 0, 0, 150}
  );
}

void draw_piece(
  const TetrominoMap& map,
  Color color,
  int horizontalOffset,
  int verticalOffset,
  const DrawingDetails& draw_d
) {
  for (auto&& coord : map) {
    int x = coord.x + horizontalOffset;
    int y = coord.y + verticalOffset;
    draw_block_pretty(x, y, draw_d, color);
  }
}

void draw_piece_danger(
  const FallingPiece& piece, const DrawingDetails& details
) {
  for (auto&& coord : piece.tetromino_map) {
    int x = coord.x + piece.x_position;
    int y = coord.y + piece.y_position;
    draw_block_danger(x, y, details);
  }
}
}; // namespace

void Playfield::draw_tetrion(const DrawingDetails& draw_d) const {
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

void Playfield::draw_tetrion_pieces(const DrawingDetails& draw_d) const {
  static constexpr auto X_DANGER_RANGE =
    std::views::iota(WIDTH / 2 - 2, WIDTH / 2 + 2);
  static constexpr auto Y_DANGER_RANGE =
    std::views::iota(INITIAL_VERTICAL_POSITION, INITIAL_VERTICAL_POSITION + 5);

  FallingPiece ghostPiece = falling_piece;
  while (is_valid_position(grid, ghostPiece.fallen()))
    ghostPiece.fall();
  draw_piece(
    ghostPiece.tetromino_map,
    GRAY,
    ghostPiece.x_position,
    ghostPiece.y_position,
    draw_d
  );

  draw_piece(
    falling_piece.tetromino_map,
    tetromino_color(falling_piece.tetromino),
    falling_piece.x_position,
    falling_piece.y_position,
    draw_d
  );

  bool is_in_danger = sr::any_of(X_DANGER_RANGE, [this](auto x) {
    return sr::any_of(Y_DANGER_RANGE, [this, x](auto y) {
      return grid[y][x] != Tetromino::Empty;
    });
  });

  if (is_in_danger) {
    draw_piece_danger(
      {next_queue[0], INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION},
      draw_d
    );
  }
}

void Playfield::draw_next_queue(const DrawingDetails& draw_d) const {
  Rectangle nextTextBlock = get_block(WIDTH + 1, VISIBLE_HEIGHT, draw_d);
  Rectangle nextQueueBackground =
    get_block(WIDTH + 1, VISIBLE_HEIGHT + 2, draw_d);
  nextQueueBackground.width = draw_d.block_length * 6;
  nextQueueBackground.height =
    draw_d.block_length * (3 * (NextQueue::NEXT_COMING_SIZE) + 1);
  DrawRectangleRec(nextQueueBackground, draw_d.PIECES_BACKGROUND_COLOR);
  DrawRectangleLinesEx(
    nextQueueBackground, draw_d.block_length / 4, draw_d.PIECE_BOX_COLOR
  );
  DrawText(
    "NEXT",
    nextTextBlock.x,
    nextTextBlock.y,
    draw_d.font_size,
    draw_d.INFO_TEXT_COLOR
  );
  for (int id = 0; id < NextQueue::NEXT_COMING_SIZE; ++id) {
    Tetromino currentTetromino = next_queue[id];
    draw_piece(
      initial_tetromino_map(currentTetromino),
      tetromino_color(currentTetromino),
      WIDTH + 3,
      3 * (id + 1) + VISIBLE_HEIGHT + 1,
      draw_d
    );
  }
}

void Playfield::draw_hold_piece(const DrawingDetails& draw_d) const {
  Rectangle holdTextBlock = get_block(-7, VISIBLE_HEIGHT, draw_d);
  DrawText(
    "HOLD",
    holdTextBlock.x,
    holdTextBlock.y,
    draw_d.font_size,
    draw_d.INFO_TEXT_COLOR
  );
  Rectangle holdPieceBackground = get_block(-7, VISIBLE_HEIGHT + 2, draw_d);
  holdPieceBackground.width = draw_d.block_length * 6;
  holdPieceBackground.height = draw_d.block_length * 4;
  DrawRectangleRec(holdPieceBackground, draw_d.PIECES_BACKGROUND_COLOR);
  DrawRectangleLinesEx(
    holdPieceBackground, draw_d.block_length / 4, draw_d.PIECE_BOX_COLOR
  );

  Color holdColor = can_swap ? tetromino_color(holding_piece) :
                               draw_d.UNAVAILABLE_HOLD_PIECE_COLOR;
  draw_piece(
    initial_tetromino_map(holding_piece),
    holdColor,
    -5,
    4 + VISIBLE_HEIGHT,
    draw_d
  );
}

std::pair<const char*, Color> message_info(MessageType message) {
  switch (message) {
  case MessageType::Single:
    return {"SINGLE", {0, 0, 0, 255}};
  case MessageType::Double:
    return {"DOUBLE", {235, 149, 52, 255}};
  case MessageType::Triple:
    return {"TRIPLE", {88, 235, 52, 255}};
  case MessageType::Tetris:
    return {"TETRIS", {52, 164, 236, 255}};
  case MessageType::AllClear:
    return {"ALL\nCLEAR", {235, 52, 213, 255}};
  default:
    return {"", BLANK};
  }
}

void Playfield::draw_info(const DrawingDetails& draw_d) const {
  if (message.timer > 0) {
    Rectangle clear_rect = get_block(draw_d.LEFT_BORDER, HEIGHT - 4, draw_d);
    const float color_scalar =
      static_cast<float>(message.timer) / LineClearMessage::DURATION;

    auto [line_clear_msg, text_color] = message_info(message.message);
    unsigned char alpha = (255 * color_scalar);
    text_color.a = alpha;
    DrawText(
      line_clear_msg, clear_rect.x, clear_rect.y, draw_d.font_size, text_color
    );

    if (message.spin_type != SpinType::No) {
      Color spin_color = tetromino_color(Tetromino::T);
      spin_color.a = alpha;
      Rectangle spin_rect = get_block(draw_d.LEFT_BORDER, HEIGHT - 6, draw_d);
      DrawText("TSPIN", spin_rect.x, spin_rect.y, draw_d.font_size, spin_color);
      if (message.spin_type == SpinType::Mini) {
        Rectangle min_block = get_block(draw_d.LEFT_BORDER, HEIGHT - 7, draw_d);
        DrawText(
          "MINI", min_block.x, min_block.y, draw_d.font_size_small, spin_color
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

void Playfield::draw(const DrawingDetails& draw_d) const {
  draw_tetrion(draw_d);
  draw_tetrion_pieces(draw_d);
  draw_next_queue(draw_d);
  draw_hold_piece(draw_d);
  draw_info(draw_d);
}
