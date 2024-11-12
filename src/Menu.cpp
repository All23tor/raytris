#include "Menu.hpp"
#include "raylib.h"
#include <format>

Menu::Menu()
    : fullScreenWidth(GetMonitorWidth(GetCurrentMonitor())),
      fullScreenHeight(GetMonitorHeight(GetCurrentMonitor())),
      windowWidth(INITIAL_WIDTH), windowHeight(INITIAL_HEIGHT) {}

const std::pair<int, int> Menu::getResolution(Resolutions resolution) const {
  switch (resolution) {
  case Resolutions::SMALL:
    return {INITIAL_WIDTH, INITIAL_HEIGHT};
  case Resolutions::MEDIUM:
    return {960, 540};
  case Resolutions::BIG:
    return {1280, 720};
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
  DrawText("RAYTRIS",
           (windowWidth - MeasureText("RAYTRIS", fontSize * 2)) / 2.0,
           windowHeight / 2.0 - 3 * fontSize, fontSize * 2, RED);
  std::string score = std::format("{} x {}", windowWidth,windowHeight);
  DrawText(score.c_str(), (windowWidth - MeasureText(score.c_str(), fontSize)) / 2.0,
           windowHeight / 2, fontSize, BLUE);
  DrawText("Press F to resize",
           (windowWidth - MeasureText("Press F to resize", fontSize)) / 2.0,
           windowHeight / 2.0 + fontSize, fontSize, BLACK);
  DrawText("Press Enter to Play",
           (windowWidth - MeasureText("Press Enter to play", fontSize)) / 2.0,
           windowHeight / 2.0 + 2 * fontSize, fontSize, BLACK);
}

void Menu::update() {
  if (IsKeyPressed(KEY_F))
    resizeScreen();
}

Menu::ExitCode Menu::run() {
  while (!IsKeyPressed(KEY_ENTER) && !IsKeyPressed(KEY_ESCAPE)) {
    update();
    BeginDrawing();
    draw();
    EndDrawing();
  }

  ExitCode exitCode = IsKeyDown(KEY_ENTER) ? ExitCode::Game : ExitCode::Exit;
  BeginDrawing();
  EndDrawing();

  return exitCode;
}
