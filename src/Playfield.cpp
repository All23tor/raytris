#include "Playfield.hpp"
#include "DrawingDetails.hpp"
#include "FallingPiece.hpp"
#include "HandlingSettings.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <ranges>

static constexpr std::size_t INITIAL_HORIZONTAL_POSITION =
  (Playfield::WIDTH - 1) / 2;
static constexpr std::size_t INITIAL_VERTICAL_POSITION =
  Playfield::VISIBLE_HEIGHT - 1;

Playfield::Playfield() :
  falling_piece(
    Tetromino::Empty, INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION
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

namespace {
std::ostream& operator<<(
  std::ostream& out,
  const std::array<std::array<Tetromino, Playfield::WIDTH>, Playfield::HEIGHT>&
    grid
) {
  for (const auto& row : grid) {
    for (const auto& mino : row) {
      out << static_cast<int>(mino) << ' ';
    }
    out << '\n';
  }
  return out;
}

std::istream& operator>>(
  std::istream& in,
  std::array<std::array<Tetromino, Playfield::WIDTH>, Playfield::HEIGHT>& grid
) {
  for (auto& row : grid) {
    for (auto& mino : row) {
      int input_mino;
      in >> input_mino;
      mino = static_cast<Tetromino>(input_mino);
    }
  }
  return in;
}

std::ostream& operator<<(std::ostream& out, const FallingPiece& fp) {
  out << static_cast<int>(fp.tetromino) << ' '
      << static_cast<int>(fp.orientation) << '\n';
  out << static_cast<int>(fp.x_position) << ' '
      << static_cast<int>(fp.y_position) << '\n';
  for (const auto& coord : fp.tetromino_map) {
    out << static_cast<int>(coord.x) << ' ' << static_cast<int>(coord.y)
        << '\n';
  }
  return out;
}

std::istream& operator>>(std::istream& in, FallingPiece& fp) {
  int input_tetromino, input_orientation, input_horizontal, input_vertical;
  in >> input_tetromino >> input_orientation;
  in >> input_horizontal >> input_vertical;
  fp.tetromino = static_cast<Tetromino>(input_tetromino);
  fp.orientation = static_cast<Orientation>(input_orientation);
  fp.x_position = input_horizontal;
  fp.y_position = input_vertical;

  for (auto& coord : fp.tetromino_map) {
    int input_coord_x, input_coord_y;
    in >> input_coord_x >> input_coord_y;
    coord = {
      static_cast<char>(input_coord_x), static_cast<char>(input_coord_y)
    };
  }
  return in;
}

std::ostream& operator<<(std::ostream& out, const Tetromino& tetromino) {
  return out << static_cast<int>(tetromino) << '\n';
}

std::istream& operator>>(std::istream& in, Tetromino& tetromino) {
  int input_tetromino;
  in >> input_tetromino;
  tetromino = static_cast<Tetromino>(input_tetromino);
  return in;
}

std::ostream& operator<<(std::ostream& out, const LineClearMessage& message) {
  return out << static_cast<int>(message.message)
             << static_cast<int>(message.timer)
             << static_cast<int>(message.spin_type) << '\n';
}

std::istream& operator>>(std::istream& in, LineClearMessage& message) {
  int input_message, input_timer, input_spin_type;
  in >> input_message >> input_timer >> input_spin_type;
  message.message = static_cast<MessageType>(input_message);
  message.timer = input_timer;
  message.spin_type = static_cast<SpinType>(input_spin_type);
  return in;
}
} // namespace

std::ostream& operator<<(std::ostream& out, const Playfield& p) {
  out << p.grid << '\n';
  out << p.falling_piece << '\n';
  out << p.holding_piece << '\n';
  out << p.next_queue << '\n';
  out << p.can_swap << '\n';
  out << p.frames_since_last_fall << '\n';
  out << p.lock_delay_frames << '\n';
  out << p.lock_delay_moves << '\n';
  out << p.frames_pressed << '\n';
  out << p.combo << '\n';
  out << p.has_lost << '\n';
  out << p.score << '\n';
  out << p.b2b << '\n';
  out << p.last_move_rotation << '\n';
  out << p.message << '\n';
  return out;
}

std::istream& operator>>(std::istream& in, Playfield& p) {
  in >> p.grid;
  in >> p.falling_piece;
  in >> p.holding_piece;
  in >> p.next_queue;
  in >> p.can_swap;
  in >> p.frames_since_last_fall;
  in >> p.lock_delay_frames;
  in >> p.lock_delay_moves;
  in >> p.frames_pressed;
  in >> p.combo;
  in >> p.has_lost;
  in >> p.score;
  in >> p.b2b;
  in >> p.last_move_rotation;
  in >> p.message;
  return in;
}

bool Playfield::lost() const {
  return has_lost;
}

static bool is_valid_position(const auto& grid, const FallingPiece& piece) {
  int h = piece.x_position, v = piece.y_position;
  return std::ranges::all_of(piece.tetromino_map, [&grid, h, v](const auto& c) {
    int i = c.x + h;
    int j = c.y + v;
    return i >= 0 && i < Playfield::WIDTH && j >= 0 && j < Playfield::HEIGHT &&
      grid[j][i] == Tetromino::Empty;
  });
}

bool Playfield::tryShifting(Shift shift) {
  const FallingPiece shiftedPiece = falling_piece.shifted(shift);
  if (is_valid_position(grid, shiftedPiece)) {
    falling_piece = shiftedPiece;
    lock_delay_frames = 0;
    lock_delay_moves += 1;
    return true;
  }

  return false;
}

namespace {
OffsetTable calculateOffsetDifferences(
  const OffsetTable& startOffsets, const OffsetTable& endOffsets
) {
  OffsetTable result;
  std::transform(
    startOffsets.begin(),
    startOffsets.end(),
    endOffsets.begin(),
    result.begin(),
    [](const CoordinatePair& p1, const CoordinatePair& p2) -> CoordinatePair {
      return {
        static_cast<signed char>(p1.x - p2.x),
        static_cast<signed char>(p1.y - p2.y)
      };
    }
  );
  return result;
}
} // namespace

bool Playfield::tryRotating(RotationType rotationType) {
  const OffsetTable& startOffsetValues = get_offset_table(falling_piece);
  const FallingPiece rotatedPiece = falling_piece.rotated(rotationType);
  const OffsetTable& endOffsetValues = get_offset_table(rotatedPiece);
  const OffsetTable trueOffsets =
    calculateOffsetDifferences(startOffsetValues, endOffsetValues);

  for (const CoordinatePair& offset : trueOffsets) {
    const FallingPiece translatedPiece = rotatedPiece.translated(offset);
    if (is_valid_position(grid, translatedPiece)) {
      falling_piece = translatedPiece;
      lock_delay_frames = 0;
      lock_delay_moves += 1;
      return true;
    }
  }
  return false;
}

SpinType Playfield::isSpin() const {
  if (!last_move_rotation)
    return SpinType::No;
  TetrominoMap corners = {{{-1, -1}, {1, -1}, {1, 1}, {-1, 1}}};
  std::array<CoordinatePair, 2> frontCorners, backCorners;
  switch (falling_piece.orientation) {
  case Orientation::Up:
    frontCorners = {{corners[0], corners[1]}};
    backCorners = {{corners[2], corners[3]}};
    break;
  case Orientation::Right:
    frontCorners = {{corners[1], corners[2]}};
    backCorners = {{corners[3], corners[0]}};
    break;
  case Orientation::Down:
    frontCorners = {{corners[2], corners[3]}};
    backCorners = {{corners[0], corners[1]}};
    break;
  case Orientation::Left:
    frontCorners = {{corners[3], corners[0]}};
    backCorners = {{corners[1], corners[2]}};
    break;
  }

  int frontCornerCount = std::count_if(
    frontCorners.begin(),
    frontCorners.end(),
    [&](const CoordinatePair& coordinates) {
      int i = falling_piece.x_position + coordinates.x;
      int j = falling_piece.y_position + coordinates.y;
      return i < 0 || i >= WIDTH || j < 0 || j >= HEIGHT ||
        grid[j][i] != Tetromino::Empty;
    }
  );

  int backCornerCount = std::count_if(
    backCorners.begin(),
    backCorners.end(),
    [&](const CoordinatePair& coordinates) {
      int i = falling_piece.x_position + coordinates.x;
      int j = falling_piece.y_position + coordinates.y;
      return i < 0 || i >= WIDTH || j < 0 || j >= HEIGHT ||
        grid[j][i] != Tetromino::Empty;
    }
  );

  if (frontCornerCount + backCornerCount < 3)
    return SpinType::No;
  if (frontCornerCount == 2)
    return SpinType::Proper;
  return SpinType::Mini;
}

void Playfield::solidify() {
  bool anyMinoSolidifedAboveVisibleHeight = false;

  for (const CoordinatePair& pair : falling_piece.tetromino_map) {
    int i = pair.x + falling_piece.x_position;
    int j = pair.y + falling_piece.y_position;
    grid[j][i] = falling_piece.tetromino;

    if (j >= VISIBLE_HEIGHT)
      anyMinoSolidifedAboveVisibleHeight = true;
  }

  clearLines();
  replaceNextPiece();
  can_swap = true;

  bool canSpawnPiece = std::all_of(
    falling_piece.tetromino_map.begin(),
    falling_piece.tetromino_map.end(),
    [&](const CoordinatePair& coordinates) {
      int i = coordinates.x + falling_piece.x_position;
      int j = coordinates.y + falling_piece.y_position;
      return grid[j][i] == Tetromino::Empty;
    }
  );

  has_lost = !anyMinoSolidifedAboveVisibleHeight || !canSpawnPiece;
}

void Playfield::clearLines() {
  std::array<std::size_t, 4> rowsToClear;
  std::size_t numberRowsToClear = 0;
  for (std::size_t j = 0; j < HEIGHT && numberRowsToClear != 4; ++j) {
    bool isRowFull = std::all_of(grid[j].begin(), grid[j].end(), [](auto mino) {
      return mino != Tetromino::Empty;
    });

    if (isRowFull) {
      rowsToClear[numberRowsToClear] = j;
      numberRowsToClear++;
    }
  }

  SpinType spinType =
    (falling_piece.tetromino == Tetromino::T) ? isSpin() : SpinType::No;
  for (std::size_t index = 0; index < numberRowsToClear; index++) {
    std::size_t rowIndex = rowsToClear[index];
    std::copy_backward(
      grid.begin(), grid.begin() + rowIndex, grid.begin() + rowIndex + 1
    );
    grid[0].fill(Tetromino::Empty);
  }
  updateScore(numberRowsToClear, spinType);
}

void Playfield::updateScore(std::size_t clearedLines, SpinType spinType) {
  if (clearedLines == 0) {
    combo = 0;
  } else {
    combo += 1;
    score += combo * 50;
    if ((clearedLines == 4) || (spinType != SpinType::No)) {
      b2b += 1;
    } else {
      b2b = 0;
    }
  }

  float b2bFactor = (b2b >= 2) ? 1.5f : 1.0f;

  if (spinType == SpinType::Proper) {
    if (clearedLines == 1) {
      message = MessageType::Single;
      score += 800 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::Double;
      score += 1200 * b2bFactor;
    } else if (clearedLines == 3) {
      message = MessageType::Triple;
      score += 1600 * b2bFactor;
    } else {
      message = MessageType::Empty;
      score += 400 * b2bFactor;
    }
    message.spin_type = SpinType::Proper;
  } else if (spinType == SpinType::Mini) {
    if (clearedLines == 1) {
      message = MessageType::Single;
      score += 200 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::Double;
      score += 400 * b2bFactor;
    } else {
      message = MessageType::Empty;
      score += 100 * b2bFactor;
    }
    message.spin_type = SpinType::Mini;
  } else {
    if (clearedLines == 1) {
      message = MessageType::Single;
      score += 100 * b2bFactor;
    } else if (clearedLines == 2) {
      message = MessageType::Double;
      score += 300 * b2bFactor;
    } else if (clearedLines == 3) {
      message = MessageType::Triple;
      score += 500 * b2bFactor;
    } else if (clearedLines == 4) {
      message = MessageType::Tetris;
      score += 800 * b2bFactor;
    }
  }

  static constexpr bool (*is_all_clear)(const decltype(grid)&) =
    [](const auto& grid) {
      return std::ranges::all_of(grid, [](const auto& row) {
        return std::ranges::all_of(row, [](Tetromino mino) {
          return mino == Tetromino::Empty;
        });
      });
    };

  if (is_all_clear(grid)) {
    message = MessageType::AllClear;
    score += 3500 * b2bFactor;
  }
}

FallingPiece Playfield::get_ghost_piece() const {
  FallingPiece ghostPiece = falling_piece;
  while (is_valid_position(grid, ghostPiece.fallen())) {
    ghostPiece.fall();
  }
  return ghostPiece;
}

void Playfield::swapTetromino() {
  Tetromino currentTetromino = falling_piece.tetromino;
  falling_piece = FallingPiece(
    holding_piece, INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION
  );
  holding_piece = currentTetromino;
  can_swap = false;
  frames_since_last_fall = 0;
  lock_delay_frames = 0;
  lock_delay_moves = 0;
}

void Playfield::replaceNextPiece() {
  Tetromino newTetromino = next_queue.getNextTetromino();
  falling_piece = FallingPiece(
    newTetromino, INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION
  );
  frames_since_last_fall = 0;
  lock_delay_frames = 0;
  lock_delay_moves = 0;
}

void Playfield::updateTimers() {
  frames_since_last_fall += 1;
  lock_delay_frames += 1;
  if (message.timer > 0)
    message.timer -= 1;
}

void Playfield::handleSpecialInput(const Controller& controller) {
  if (controller.swap() && can_swap) {
    swapTetromino();
    last_move_rotation = false;
  }
}

void Playfield::handleShiftInput(
  const Controller& controller, const HandlingSettings& settings
) {
  if (controller.left()) {
    if (tryShifting(Shift::Left))
      last_move_rotation = false;
  } else if (controller.right()) {
    if (tryShifting(Shift::Right))
      last_move_rotation = false;
  }

  if (controller.left_das()) {
    if (frames_pressed < 0)
      frames_pressed = 0;
    frames_pressed += 1;
    while (frames_pressed > settings.das) {
      if (!tryShifting(Shift::Left))
        break;
      else
        last_move_rotation = false;
    }
  } else if (controller.right_das()) {
    if (frames_pressed > 0)
      frames_pressed = 0;
    frames_pressed -= 1;
    while (-frames_pressed > settings.das) {
      if (!tryShifting(Shift::Right))
        break;
      else
        last_move_rotation = false;
    }
  } else {
    frames_pressed = 0;
  }
}

void Playfield::handleRotationInput(const Controller& controller) {
  if (controller.clockwise()) {
    if (tryRotating(RotationType::Clockwise))
      last_move_rotation = true;
  } else if (controller.counter_clockwise()) {
    if (tryRotating(RotationType::CounterClockwise))
      last_move_rotation = true;
  } else if (controller.one_eighty()) {
    if (tryRotating(RotationType::OneEighty))
      last_move_rotation = true;
  }
}

bool Playfield::handleDropInput(
  const Controller& controller, const HandlingSettings& settings
) {
  if (controller.chech_hard_drop()) {
    // Hard Drop
    if (is_valid_position(grid, falling_piece.fallen()))
      last_move_rotation = false;
    falling_piece = get_ghost_piece();
    solidify();
    return true;
  }

  bool isFallStep = false;
  if (controller.soft_drop()) {
    // Soft Drop
    if (frames_since_last_fall >= settings.softDropFrames) {
      frames_since_last_fall = 0;
      isFallStep = true;
    }
  } else if (frames_since_last_fall >= settings.gravityFrames) {
    // Gravity
    frames_since_last_fall = 0;
    isFallStep = true;
  }

  if (is_valid_position(grid, falling_piece.fallen())) {
    // Is not touching ground
    if (isFallStep) {
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
    solidify();
    return true;
  }
  return false;
}

bool Playfield::update(
  const Controller& controller, const HandlingSettings& settings
) {
  if (has_lost)
    return false;
  handleSpecialInput(controller);
  updateTimers();
  next_queue.pushNewBagIfNeeded();
  if (falling_piece.tetromino == Tetromino::Empty)
    replaceNextPiece();
  handleShiftInput(controller, settings);
  handleRotationInput(controller);
  return handleDropInput(controller, settings);
}

namespace {
constexpr Color getTetrominoColor(Tetromino tetromino) {
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

inline Rectangle
getBlockRectangle(int i, int j, const DrawingDetails& d_details) {
  return {
    d_details.position.x + i * d_details.block_length,
    d_details.position.y +
      (j - static_cast<int>(Playfield::VISIBLE_HEIGHT)) *
        d_details.block_length,
    d_details.block_length,
    d_details.block_length
  };
}

void drawBlockPretty(int i, int j, const DrawingDetails& details, Color fill) {
  if (fill.a == 0)
    return;

  Rectangle rec = getBlockRectangle(i, j, details);
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

void drawBlockDanger(int i, int j, const DrawingDetails& details) {
  Rectangle rec = getBlockRectangle(i, j, details);
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

void drawPiece(
  const TetrominoMap& map,
  Color color,
  int horizontalOffset,
  int verticalOffset,
  const DrawingDetails& d_details
) {
  for (const CoordinatePair& coordinates : map) {
    int i = coordinates.x + horizontalOffset;
    int j = coordinates.y + verticalOffset;
    drawBlockPretty(i, j, d_details, color);
  }
}

void drawPieceDanger(const FallingPiece& piece, const DrawingDetails& details) {
  for (const CoordinatePair& coordinates : piece.tetromino_map) {
    int i = coordinates.x + piece.x_position;
    int j = coordinates.y + piece.y_position;
    drawBlockDanger(i, j, details);
  }
}
}; // namespace

void Playfield::draw_tetrion(const DrawingDetails& d_details) const {
  Rectangle tetrion = Rectangle{
    d_details.position.x,
    d_details.position.y,
    d_details.block_length * WIDTH,
    d_details.block_length * VISIBLE_HEIGHT
  };
  DrawRectangleRec(tetrion, d_details.TETRION_BACKGROUND_COLOR);
  DrawRectangleLinesEx(
    tetrion, d_details.block_length / 10, d_details.GRINDLINE_COLOR
  );

  for (int i = 1; i < WIDTH; ++i) {
    Rectangle rec = getBlockRectangle(i, VISIBLE_HEIGHT, d_details);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx(
      {rec.x, rec.y},
      {rec.x, std::floor(rec.y + VISIBLE_HEIGHT * d_details.block_length)},
      d_details.block_length / 10,
      d_details.GRINDLINE_COLOR
    );
  }

  for (int j = 1; j < VISIBLE_HEIGHT; ++j) {
    Rectangle rec = getBlockRectangle(0, j + VISIBLE_HEIGHT, d_details);
    rec.x = std::floor(rec.x);
    rec.y = std::floor(rec.y);
    DrawLineEx(
      {rec.x, rec.y},
      {std::floor(rec.x + d_details.block_length * WIDTH), rec.y},
      d_details.block_length / 10,
      d_details.GRINDLINE_COLOR
    );
  }

  for (int j = 0; j < HEIGHT; ++j) {
    for (int i = 0; i < WIDTH; ++i) {
      drawBlockPretty(i, j, d_details, getTetrominoColor(grid[j][i]));
    }
  }
}

bool Playfield::isInDanger() const {
  static constexpr auto dangerous_x_range =
    std::views::iota(WIDTH / 2 - 2, WIDTH / 2 + 2);
  static constexpr auto dangerous_y_range =
    std::views::iota(INITIAL_VERTICAL_POSITION, INITIAL_VERTICAL_POSITION + 5);

  return std::ranges::any_of(dangerous_x_range, [this](auto x) {
    return std::ranges::any_of(dangerous_y_range, [this, x](auto y) {
      return grid[y][x] != Tetromino::Empty;
    });
  });
}

void Playfield::draw_pieces(const DrawingDetails& d_details) const {
  const FallingPiece ghostPiece = get_ghost_piece();
  drawPiece(
    ghostPiece.tetromino_map,
    GRAY,
    ghostPiece.x_position,
    ghostPiece.y_position,
    d_details
  );
  drawPiece(
    falling_piece.tetromino_map,
    getTetrominoColor(falling_piece.tetromino),
    falling_piece.x_position,
    falling_piece.y_position,
    d_details
  );
  if (isInDanger()) {
    drawPieceDanger(
      {next_queue[0], INITIAL_HORIZONTAL_POSITION, INITIAL_VERTICAL_POSITION},
      d_details
    );
  }
}

void Playfield::draw_next_pieces(const DrawingDetails& d_details) const {
  Rectangle nextTextBlock =
    getBlockRectangle(WIDTH + 1, VISIBLE_HEIGHT, d_details);
  Rectangle nextQueueBackground =
    getBlockRectangle(WIDTH + 1, VISIBLE_HEIGHT + 2, d_details);
  nextQueueBackground.width = d_details.block_length * 6;
  nextQueueBackground.height =
    d_details.block_length * (3 * (NextQueue::NEXT_COMING_SIZE) + 1);
  DrawRectangleRec(nextQueueBackground, d_details.PIECES_BACKGROUND_COLOR);
  DrawRectangleLinesEx(
    nextQueueBackground, d_details.block_length / 4, d_details.PIECE_BOX_COLOR
  );
  DrawText(
    "NEXT",
    nextTextBlock.x,
    nextTextBlock.y,
    d_details.font_size,
    d_details.INFO_TEXT_COLOR
  );
  for (int id = 0; id < NextQueue::NEXT_COMING_SIZE; ++id) {
    Tetromino currentTetromino = next_queue[id];
    drawPiece(
      initial_tetromino_map(currentTetromino),
      getTetrominoColor(currentTetromino),
      WIDTH + 3,
      3 * (id + 1) + VISIBLE_HEIGHT + 1,
      d_details
    );
  }
}

void Playfield::draw_hold_piece(const DrawingDetails& d_details) const {
  Rectangle holdTextBlock = getBlockRectangle(-7, VISIBLE_HEIGHT, d_details);
  DrawText(
    "HOLD",
    holdTextBlock.x,
    holdTextBlock.y,
    d_details.font_size,
    d_details.INFO_TEXT_COLOR
  );
  Rectangle holdPieceBackground =
    getBlockRectangle(-7, VISIBLE_HEIGHT + 2, d_details);
  holdPieceBackground.width = d_details.block_length * 6;
  holdPieceBackground.height = d_details.block_length * 4;
  DrawRectangleRec(holdPieceBackground, d_details.PIECES_BACKGROUND_COLOR);
  DrawRectangleLinesEx(
    holdPieceBackground, d_details.block_length / 4, d_details.PIECE_BOX_COLOR
  );

  Color holdColor = can_swap ? getTetrominoColor(holding_piece) :
                               d_details.UNAVAILABLE_HOLD_PIECE_COLOR;
  drawPiece(
    initial_tetromino_map(holding_piece),
    holdColor,
    -5,
    4 + VISIBLE_HEIGHT,
    d_details
  );
}

void Playfield::draw_line_clear_message(const DrawingDetails& d_details) const {
  if (message.timer <= 0)
    return;

  Rectangle clearTextBlock =
    getBlockRectangle(d_details.LEFT_BORDER, HEIGHT - 4, d_details);
  const float colorScaleFactor =
    static_cast<float>(message.timer) / LineClearMessage::DURATION;
  std::string lineClearMessage;

  Color textColor = BLANK;
  unsigned char alpha = static_cast<unsigned char>(255 * colorScaleFactor);
  switch (message.message) {
  case MessageType::Single:
    lineClearMessage = "SINGLE";
    textColor = {0, 0, 0, alpha};
    break;
  case MessageType::Double:
    lineClearMessage = "DOUBLE";
    textColor = {235, 149, 52, alpha};
    break;
  case MessageType::Triple:
    lineClearMessage = "TRIPLE";
    textColor = {88, 235, 52, alpha};
    break;
  case MessageType::Tetris:
    lineClearMessage = "TETRIS";
    textColor = {52, 164, 236, alpha};
    break;
  case MessageType::AllClear:
    lineClearMessage = "ALL\nCLEAR";
    textColor = {235, 52, 213, alpha};
    break;
  case MessageType::Empty:
    lineClearMessage = "";
  }
  DrawText(
    lineClearMessage.c_str(),
    clearTextBlock.x,
    clearTextBlock.y,
    d_details.font_size,
    textColor
  );

  if (message.spin_type == SpinType::No)
    return;
  Color tSpinTextColor = getTetrominoColor(Tetromino::T);
  tSpinTextColor.a = alpha;
  Rectangle tSpinTextBlock =
    getBlockRectangle(d_details.LEFT_BORDER, HEIGHT - 6, d_details);
  DrawText(
    "TSPIN",
    tSpinTextBlock.x,
    tSpinTextBlock.y,
    d_details.font_size,
    tSpinTextColor
  );
  if (message.spin_type == SpinType::Mini) {
    Rectangle miniTSpinTextBlock =
      getBlockRectangle(d_details.LEFT_BORDER, HEIGHT - 7, d_details);
    DrawText(
      "MINI",
      miniTSpinTextBlock.x,
      miniTSpinTextBlock.y,
      d_details.font_size_small,
      tSpinTextColor
    );
  }
}

void Playfield::draw_combo(const DrawingDetails& d_details) const {
  if (combo < 2)
    return;
  Rectangle comboTextBlock =
    getBlockRectangle(d_details.LEFT_BORDER, HEIGHT - 10, d_details);
  std::string comboText = std::format("{}", combo);
  DrawText(
    "COMBO ", comboTextBlock.x, comboTextBlock.y, d_details.font_size, BLUE
  );
  DrawText(
    comboText.c_str(),
    comboTextBlock.x + MeasureText("COMBO ", d_details.font_size),
    comboTextBlock.y,
    d_details.font_size,
    BLUE
  );
}

void Playfield::draw_b2b(const DrawingDetails& d_details) const {
  if (b2b < 2)
    return;
  Rectangle b2bTextBlock =
    getBlockRectangle(d_details.LEFT_BORDER, HEIGHT - 12, d_details);
  std::string b2bText = std::format("{}", b2b - 1);
  DrawText("B2B ", b2bTextBlock.x, b2bTextBlock.y, d_details.font_size, BLUE);
  DrawText(
    b2bText.c_str(),
    b2bTextBlock.x + MeasureText("B2B ", d_details.font_size),
    b2bTextBlock.y,
    d_details.font_size,
    BLUE
  );
}

void Playfield::draw_score(const DrawingDetails& d_details) const {
  Rectangle scoreNumberBlock =
    getBlockRectangle(WIDTH + 1, HEIGHT - 2, d_details);
  std::string scoreText = std::format("{:09}", score);
  DrawText(
    scoreText.c_str(),
    scoreNumberBlock.x,
    scoreNumberBlock.y + d_details.block_length * 0.5,
    d_details.font_size,
    d_details.INFO_TEXT_COLOR
  );
}

void Playfield::draw(const DrawingDetails& d_details) const {
  draw_tetrion(d_details);
  draw_pieces(d_details);
  draw_next_pieces(d_details);
  draw_hold_piece(d_details);
  draw_line_clear_message(d_details);
  draw_combo(d_details);
  draw_b2b(d_details);
  draw_score(d_details);
}
