#include "Menu.hpp"
#include "raylib.h"
#include <format>
#include <utility>

namespace {
  std::string_view optionToString(Menu::Option option) {
    switch (option) {
      case Menu::Option::SinglePlayer: return "Single Player";
      case Menu::Option::TwoPlayers: return "Two Players";
    }
    return "";
  }

  std::pair<int, int> getResolution(Resolution resolution) {
    switch (resolution) {
    case Resolution::Small:
      return { Menu::InitialWidth, Menu::InitialHeight };
    case Resolution::Medium:
      return { 960, 540 };
    case Resolution::Big:
      return { 1280, 720 };
    case Resolution::FullScreen: {
      int monitor = GetCurrentMonitor();
      return { GetMonitorWidth(monitor), GetMonitorHeight(monitor) };
    }
    default:
      return { 0, 0 };
    }
  }

  Resolution nextResolution(Resolution res) {
    return static_cast<Resolution>((std::to_underlying(res) + 1) % (std::to_underlying(Resolution::FullScreen) + 1));
  }
};

void Menu::resizeScreen() {
  resolution = nextResolution(resolution);
  windowResolution = getResolution(resolution);

  if (IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  SetWindowSize(windowResolution.first, windowResolution.second);

  if (resolution == Resolution::FullScreen) {
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
  std::string gameMode = "Mode: ";
  gameMode += optionToString(selectedOption);
  DrawText(gameMode.c_str(), (windowWidth - MeasureText(gameMode.c_str(), fontSize)) / 2.0,
    windowHeight / 2.0 - 1 * fontSize, fontSize, DARKBLUE);
  std::string resolution = std::format("{} x {}", windowWidth, windowHeight);
  DrawText(resolution.c_str(), (windowWidth - MeasureText(resolution.c_str(), fontSize)) / 2.0,
    windowHeight / 2.0 + 1 * fontSize, fontSize, BLUE);
  DrawText("Press F to resize",
    (windowWidth - MeasureText("Press F to resize", fontSize)) / 2.0,
    windowHeight / 2.0 + 2 * fontSize, fontSize, BLACK);
  DrawText("Press Enter to Play",
    (windowWidth - MeasureText("Press Enter to play", fontSize)) / 2.0,
    windowHeight / 2.0 + 3 * fontSize, fontSize, BLACK);
}

void Menu::update() {
  if (IsKeyPressed(KEY_F))
    resizeScreen();
  if (IsKeyPressed(KEY_UP)) {
    selectedOption = static_cast<Option>((std::to_underlying(selectedOption) + 1) % std::to_underlying(Option::Exit));
  }
  if (IsKeyPressed(KEY_DOWN)) {
    selectedOption = static_cast<Option>((std::to_underlying(selectedOption) - 1) % std::to_underlying(Option::Exit));
  }
}

Menu::Option Menu::run() {
  while (!IsKeyPressed(KEY_ENTER) && !IsKeyPressed(KEY_ESCAPE)) {
    update();
    BeginDrawing();
    draw();
    EndDrawing();
  }

  Option chosenOption = IsKeyDown(KEY_ENTER) ? selectedOption : Option::Exit;
  BeginDrawing();
  EndDrawing();

  return chosenOption;
}
