#include "Menu.hpp"
#include "raylib.h"
#include <format>

Menu::Menu(): windowResolution{InitialWidth, InitialHeight} {}

const std::pair<int, int> Menu::getResolution(Resolutions resolution) const {
  switch (resolution) {
  case Resolutions::Small:
    return { InitialWidth, InitialHeight };
  case Resolutions::Medium:
    return { 960, 540 };
  case Resolutions::Big:
    return { 1280, 720 };
  case Resolutions::FullScreen: {
    return { GetScreenWidth(), GetScreenHeight() };
  }
  default:
    return { 0, 0 };
  }
}

static Resolutions nextResolution(Resolutions res) {
  switch (res) {
  case Resolutions::Small: return Resolutions::Medium;
  case Resolutions::Medium: return Resolutions::Big;
  case Resolutions::Big: return Resolutions::FullScreen;
  case Resolutions::FullScreen: return Resolutions::Small;
  default: return Resolutions::Small;
  }
}

void Menu::resizeScreen() {
  resolution = nextResolution(resolution);
  windowResolution = getResolution(resolution);

  if (IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  SetWindowSize(windowResolution.first, windowResolution.second);

  if (resolution == Resolutions::FullScreen) {
    ToggleFullscreen();
  }
}

void Menu::draw() const {
  const int windowWidth = windowResolution.first;
  const int windowHeight = windowResolution.second;
  const float fontSize = windowHeight / 10.0;

  ClearBackground(LIGHTGRAY);
  DrawText("RAYTRIS",
    (windowWidth - MeasureText("RAYTRIS", fontSize * 2)) / 2.0,
    windowHeight / 2.0 - 3 * fontSize, fontSize * 2, RED);
  std::string resolution = std::format("{} x {}", windowWidth, windowHeight);
  DrawText(resolution.c_str(), (windowWidth - MeasureText(resolution.c_str(), fontSize)) / 2.0,
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
