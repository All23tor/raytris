#include "Menu.hpp"
#include "raylib.h"
#include <string>
#include <string_view>
#include <utility>

namespace {
  std::string_view optionToString(Menu::Option option) {
    switch (option) {
      case Menu::Option::SinglePlayer: return "Single Player";
      case Menu::Option::TwoPlayers: return "Two Players";
      case Menu::Option::Settings: return "Settings";
      case Menu::Option::Exit: return "Exit";
    }
    return "";
  }
};

void Menu::draw() const {
  const int windowWidth = GetScreenWidth();
  const int windowHeight = GetScreenHeight();
  const float fontSize = windowHeight / 10.0;

  ClearBackground(LIGHTGRAY);
  DrawText("RAYTRIS",
    (windowWidth - MeasureText("RAYTRIS", fontSize * 2)) / 2.0,
    windowHeight / 2.0 - 3 * fontSize, fontSize * 2, RED);
  std::string gameMode = "Mode: ";
  gameMode += optionToString(selectedOption);
  DrawText(gameMode.c_str(), (windowWidth - MeasureText(gameMode.c_str(), fontSize)) / 2.0,
    windowHeight / 2.0 - 1 * fontSize, fontSize, DARKBLUE);
  DrawText("Press Enter to confirm",
    (windowWidth - MeasureText("Press Enter to confirm", fontSize)) / 2.0,
    windowHeight / 2.0 + 3 * fontSize, fontSize, BLACK);
}

void Menu::update() {
  if (IsKeyPressed(KEY_UP)) {
    selectedOption = static_cast<Option>((std::to_underlying(selectedOption) + 1) % std::to_underlying(Option::Exit));
  }
  if (IsKeyPressed(KEY_DOWN)) {
    selectedOption = static_cast<Option>((std::to_underlying(selectedOption) - 1 + std::to_underlying(Option::Exit)) % std::to_underlying(Option::Exit));
  }
}

bool Menu::shouldStopRunning() const {
  return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE);
}

Menu::Option Menu::getSelectedOption() const {
  return IsKeyDown(KEY_ENTER) ? selectedOption : Option::Exit;
}

Menu::Option Menu::runAndGetSelectedOption() {
  run();
  return getSelectedOption();
}
