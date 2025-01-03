#ifndef GAME_HPP
#define GAME_HPP

#include "Runnable.hpp"
#include "Playfield.hpp"

class Game : public Runnable {
protected:
  const DrawingDetails drawingDetails;
  const Controller controller;
  Playfield playfield;
  bool paused = false;

private:
  virtual bool shouldStopRunning() const override {
    return !controller.checkQuitInput() || !(paused || playfield.lost());
  }

public:
  explicit Game(const DrawingDetails&);
  explicit Game(const Controller&);
  explicit Game(const DrawingDetails&, const Controller&);

  virtual ~Game() = default;
};

#endif