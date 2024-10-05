#include "Menu.hpp"
#include "raylib.h"
#include <cstdio>

std::pair<int, int> getResolution(Resolutions resolution) {

  switch (resolution) {
  case Resolutions::SMALL:
    return {640, 480};
  case Resolutions::MEDIUM:
    return {1280, 720};
  case Resolutions::BIG:
    return {1600, 900};
  case Resolutions::FULLSCREEN: {
    int monitor = GetCurrentMonitor();
    return {GetMonitorWidth(monitor), GetMonitorHeight(monitor)};
  }
  default:
    return {0, 0};
  }
}

void Menu::toggleFullscreen() {
  resolution =
      static_cast<Resolutions>((static_cast<int>(resolution) + 1) %
                               (static_cast<int>(Resolutions::FULLSCREEN) + 1));

  if (IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  if (resolution == Resolutions::FULLSCREEN) {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    ToggleFullscreen();
  } else {
    SetConfigFlags(~FLAG_FULLSCREEN_MODE);
  }

  std::pair<int, int> resolutionSizes = getResolution(resolution);
  SetWindowSize(resolutionSizes.first, resolutionSizes.second);
}

void Menu::draw() const {
  auto [width, height] = getResolution(resolution);
  const float fontSize = height / 10.0;
  ClearBackground(LIGHTGRAY);
  char score[255];
  sprintf(score, "%d x %d", width, height);
  DrawText(score, width / 2.0 + -(float)MeasureText("YOU LOST", fontSize) / 2,
           height / 2, fontSize, BLUE);
  DrawText("Press F to resize",
           width / 2.0 + -(float)MeasureText("Press F to resize", fontSize) / 2,
           height / 2.0 + fontSize, fontSize, BLACK);
  DrawText("Press Enter to Play",
           width / 2.0 +
               -(float)MeasureText("Press Enter to play", fontSize) / 2,
           height / 2.0 + fontSize * 2, fontSize, BLACK);
}

void Menu::update() {
  if (IsKeyPressed(KEY_F))
    toggleFullscreen();
}

void Menu::run() {
  while (!IsKeyDown(KEY_ENTER)) {
    update();
    BeginDrawing();
    draw();
    EndDrawing();
  }
}
