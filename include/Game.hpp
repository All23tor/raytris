#ifndef GAME_HPP
#define GAME_HPP

#include "Playfield.hpp"

struct Game {
  const DrawingDetails drawingDetails;
  const Controller controller;
  const HandlingSettings settings;
  Playfield playfield;
  bool paused = false;

  Game(const DrawingDetails&, const Controller&, const HandlingSettings&);
  void draw() const;
  bool update();
};

#endif
