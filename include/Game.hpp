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
  Game() :
    drawingDetails(
      DrawingDetails::HeightScaleFactor * GetScreenHeight() / (Playfield::VisibleHeight),
      {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}
    ),
    controller(KeyboardControls) {}

  void run() {
    while (!controller.checkQuitInput() || !(paused || playfield.lost())) {
      update();
      BeginDrawing();
      draw();
      EndDrawing();
    }

    BeginDrawing();
    EndDrawing();
  }
};

#endif