#include "Menu.hpp"
#include "raylib.h"
#include <cstdio>

Menu::Menu()
    : fullScreenWidth(GetMonitorWidth(GetCurrentMonitor())),
      fullScreenHeight(GetMonitorHeight(GetCurrentMonitor())),
      windowWidth(INITIAL_WIDTH), windowHeight(INITIAL_HEIGHT) {}

const std::pair<int, int> Menu::getResolution(Resolutions resolution) const {
  switch (resolution) {
  case Resolutions::SMALL:
    return {INITIAL_WIDTH, INITIAL_HEIGHT};
  case Resolutions::MEDIUM:
    return {1280, 720};
  case Resolutions::BIG:
    return {1600, 900};
  case Resolutions::FULLSCREEN: {
    int monitor = GetCurrentMonitor();
    return {fullScreenWidth, fullScreenHeight};
  }
  default:
    return {0, 0};
  }
}

void Menu::resizeScreen() {
  resolution =
      static_cast<Resolutions>((static_cast<int>(resolution) + 1) %
                               (static_cast<int>(Resolutions::FULLSCREEN) + 1));

  std::pair<int, int> resolutionSizes = getResolution(resolution);
  windowWidth = resolutionSizes.first;
  windowHeight = resolutionSizes.second;
  SetWindowSize(resolutionSizes.first, resolutionSizes.second);

  if (IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  if (resolution == Resolutions::FULLSCREEN) {
    ToggleFullscreen();
  }
}

void Menu::draw() const {
  const float fontSize = windowHeight / 10.0;
  ClearBackground(LIGHTGRAY);
  char score[255];
  sprintf(score, "%d x %d", windowWidth, windowHeight);
  DrawText(score, windowWidth / 2.0 + -(float)MeasureText(score, fontSize) / 2,
           windowHeight / 2, fontSize, BLUE);
  DrawText("Press F to resize",
           windowWidth / 2.0 - MeasureText("Press F to resize", fontSize) / 2.0,
           windowHeight / 2.0 + fontSize, fontSize, BLACK);
  DrawText("Press Enter to Play",
           windowWidth / 2.0 +
               -MeasureText("Press Enter to play", fontSize) / 2.0,
           windowHeight / 2.0 + 2 * fontSize, fontSize, BLACK);
}

void Menu::update() {
  if (IsKeyPressed(KEY_F))
    resizeScreen();
}

bool Menu::run() {
  while (!IsKeyPressed(KEY_ENTER) && !IsKeyPressed(KEY_ESCAPE)) {
    update();
    BeginDrawing();
    draw();
    EndDrawing();
  }

  return IsKeyDown(KEY_ENTER);
}
