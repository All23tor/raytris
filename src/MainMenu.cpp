#include "MainMenu.hpp"
#include "raylib.h"
#include <array>
#include <utility>

namespace {
const char* optionToString(MainMenu::Option option) {
  switch (option) {
  case MainMenu::Option::SinglePlayer:
    return "Single Player";
  case MainMenu::Option::TwoPlayers:
    return "Two Players";
  case MainMenu::Option::Settings:
    return "Settings";
  case MainMenu::Option::Exit:
    return "Exit";
  }
  return "";
}
}; // namespace

void MainMenu::draw() const {
  const int windowWidth = GetScreenWidth();
  const int windowHeight = GetScreenHeight();
  const float fontSize = windowHeight / 10.0f;
  const float fontSizeBig = windowHeight / 4.0f;

  ClearBackground(LIGHTGRAY);
  DrawText("RAYTRIS",
           (windowWidth - MeasureText("RAYTRIS", fontSizeBig)) / 2.0f,
           windowHeight / 2.0f - fontSizeBig - fontSize, fontSizeBig, RED);

  static constexpr auto options = []() {
    std::array<Option, std::to_underlying(Option::Exit)> res;
    for (int i = 0; i < res.size(); i++) {
      res[i] = static_cast<Option>(i);
    }
    return res;
  }();

  const float boxWidth = 8.0f * fontSize;
  const float separation = 1.5f * fontSize;
  const float boxHeight = 1.3f * fontSize;
  for (int i = 0; i < options.size(); i++) {
    const auto s = optionToString(options[i]);
    const bool isSelected = options[i] == selectedOption;
    const Rectangle box = {(windowWidth - boxWidth) / 2.0f,
                           (windowHeight - boxHeight + fontSize) / 2.0f +
                             i * separation,
                           boxWidth, boxHeight};
    DrawRectangleRec(box, isSelected ? SKYBLUE : GRAY);
    DrawRectangleLinesEx(box, fontSize / 10.0, isSelected ? BLUE : BLACK);
    DrawText(s, (windowWidth - MeasureText(s, fontSize)) / 2.0,
             windowHeight / 2.0 + i * separation, fontSize,
             isSelected ? BLUE : BLACK);
  }
}

void MainMenu::update() {
  constexpr auto optionsSize = std::to_underlying(Option::Exit);
  if (IsKeyPressed(KEY_DOWN)) {
    selectedOption = static_cast<Option>(
      (std::to_underlying(selectedOption) + 1) % optionsSize);
  }
  if (IsKeyPressed(KEY_UP)) {
    selectedOption = static_cast<Option>(
      (std::to_underlying(selectedOption) - 1 + optionsSize) % optionsSize);
  }
}

bool MainMenu::shouldStopRunning() const {
  return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE);
}

MainMenu::Option MainMenu::getSelectedOption() const {
  return IsKeyDown(KEY_ENTER) ? selectedOption : Option::Exit;
}
