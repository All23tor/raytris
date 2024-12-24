#ifndef GAME_HPP
#define GAME_HPP

#include "Playfield.hpp"

class Game {
  const static Controller KeyboardControls;

protected:
  const DrawingDetails drawingDetails;
  const Controller controller;
  Playfield playfield;
  bool paused = false;

private:
  virtual void draw() const = 0;
  virtual void update() = 0;

public:
  Game();
  explicit Game(const DrawingDetails&);
  explicit Game(const Controller&);
  explicit Game(const DrawingDetails&, const Controller&);

  virtual ~Game() = default;

  void run();
};

#endif