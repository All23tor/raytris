#ifndef TWO_PLAYER_GAME_H
#define TWO_PLAYER_GAME_H

#include "Game.hpp"
#include "HandlingSettings.hpp"

class TwoPlayerGame {
  Game game1;
  Game game2;

  static constexpr Controller FirstPlayerControls{
    []() -> bool { return IsKeyPressed(KEY_R); },
    []() -> bool { return IsKeyPressed(KEY_LEFT_SHIFT); },
    []() -> bool { return IsKeyPressed(KEY_A); },
    []() -> bool { return IsKeyPressed(KEY_D); },
    []() -> bool { return IsKeyDown(KEY_A); },
    []() -> bool { return IsKeyDown(KEY_D); },
    []() -> bool { return IsKeyPressed(KEY_W); },
    []() -> bool { return IsKeyPressed(KEY_Q); },
    []() -> bool { return IsKeyPressed(KEY_E); },
    []() -> bool { return IsKeyPressed(KEY_C); },
    []() -> bool { return IsKeyDown(KEY_S); },
    []() -> bool { return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z); },
    []() -> bool { return IsKeyPressed(KEY_ENTER); },
    []() -> bool { return IsKeyPressed(KEY_ESCAPE); },
  };

  static constexpr Controller SecondPlayerControls{
    []() -> bool { return IsKeyPressed(KEY_R); },
    []() -> bool { return IsKeyPressed(KEY_M); },
    []() -> bool { return IsKeyPressed(KEY_LEFT); },
    []() -> bool { return IsKeyPressed(KEY_RIGHT); },
    []() -> bool { return IsKeyDown(KEY_LEFT); },
    []() -> bool { return IsKeyDown(KEY_RIGHT); },
    []() -> bool { return IsKeyPressed(KEY_UP); },
    []() -> bool { return IsKeyPressed(KEY_B); },
    []() -> bool { return IsKeyPressed(KEY_N); },
    []() -> bool { return IsKeyPressed(KEY_SPACE); },
    []() -> bool { return IsKeyDown(KEY_DOWN); },
    []() -> bool { return false; },
    []() -> bool { return false; },
    []() -> bool { return false; },
  };

private:
  void DrawPauseMenu() const;

public:
  TwoPlayerGame(const HandlingSettings&, const HandlingSettings&);
  void update();
  void draw() const;
  bool shouldStopRunning() const;
};

#endif
