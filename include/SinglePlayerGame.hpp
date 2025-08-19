#ifndef SINGLE_PLAYER_GAME_H
#define SINGLE_PLAYER_GAME_H

#include "Game.hpp"
#include <stack>

class SinglePlayerGame {
  static constexpr Controller keyboardControls{
    []() -> bool { return IsKeyPressed(KEY_R); },
    []() -> bool { return IsKeyPressed(KEY_C); },
    []() -> bool { return IsKeyPressed(KEY_LEFT); },
    []() -> bool { return IsKeyPressed(KEY_RIGHT); },
    []() -> bool { return IsKeyDown(KEY_LEFT); },
    []() -> bool { return IsKeyDown(KEY_RIGHT); },
    []() -> bool { return IsKeyPressed(KEY_UP); },
    []() -> bool { return IsKeyPressed(KEY_Z); },
    []() -> bool { return IsKeyPressed(KEY_A); },
    []() -> bool { return IsKeyPressed(KEY_SPACE); },
    []() -> bool { return IsKeyDown(KEY_DOWN); },
    []() -> bool { return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z); },
    []() -> bool { return IsKeyPressed(KEY_ENTER); },
    []() -> bool { return IsKeyPressed(KEY_ESCAPE); },
  };

  Game game;
  std::stack<Playfield> undoMoveStack;

public:
  SinglePlayerGame(const HandlingSettings&);
  ~SinglePlayerGame();
  void update();
  void draw() const;
  bool shouldStopRunning() const;
};

#endif
