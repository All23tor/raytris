#ifndef GAME_HPP
#define GAME_HPP

#include "Playfield.hpp"

struct Game {
  Playfield playfield;
  const DrawingDetails drawingDetails;
  const Controller controller;
  const HandlingSettings settings;
  bool paused = false;

  void DrawPauseMenu() const; 
  Game(const DrawingDetails&, const Controller&, const HandlingSettings&);
  void draw() const;
  bool update();
};

#endif
