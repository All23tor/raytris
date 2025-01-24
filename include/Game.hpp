#ifndef GAME_HPP
#define GAME_HPP

#include "Playfield.hpp"

class Game {
protected:
  const DrawingDetails drawingDetails;
  const Controller controller;
  Playfield playfield;
  bool paused = false;

public:
  bool shouldStopRunning() const;
  explicit Game(const DrawingDetails&);
  explicit Game(const Controller&);
  explicit Game(const DrawingDetails&, const Controller&);
};

#endif
