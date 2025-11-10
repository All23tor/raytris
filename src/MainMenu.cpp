#include "MainMenu.hpp"
#include "raylib.h"
#include <array>
#include <utility>

namespace {
const char* to_string(MainMenu::Option option) {
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
  const int width = GetScreenWidth();
  const int height = GetScreenHeight();
  const float font_size = height / 10.0f;
  const float font_size_big = height / 4.0f;

  ClearBackground(LIGHTGRAY);
  DrawText(
    "RAYTRIS",
    (width - MeasureText("RAYTRIS", font_size_big)) / 2.0f,
    height / 2.0f - font_size_big - font_size,
    font_size_big,
    RED
  );

  static constexpr auto OPTIONS = []() {
    std::array<Option, std::to_underlying(Option::Exit)> res;
    for (int i = 0; i < res.size(); i++) {
      res[i] = static_cast<Option>(i);
    }
    return res;
  }();

  const float boxWidth = 8.0f * font_size;
  const float separation = 1.5f * font_size;
  const float boxHeight = 1.3f * font_size;
  for (int i = 0; i < OPTIONS.size(); i++) {
    const auto s = to_string(OPTIONS[i]);
    const bool isSelected = OPTIONS[i] == selectedOption;
    const Rectangle box = {
      (width - boxWidth) / 2.0f,
      (height - boxHeight + font_size) / 2.0f + i * separation,
      boxWidth,
      boxHeight
    };
    DrawRectangleRec(box, isSelected ? SKYBLUE : GRAY);
    DrawRectangleLinesEx(box, font_size / 10.0, isSelected ? BLUE : BLACK);
    DrawText(
      s,
      (width - MeasureText(s, font_size)) / 2.0,
      height / 2.0 + i * separation,
      font_size,
      isSelected ? BLUE : BLACK
    );
  }
}

void MainMenu::update() {
  constexpr auto optionsSize = std::to_underlying(Option::Exit);
  if (IsKeyPressed(KEY_DOWN)) {
    selectedOption = static_cast<Option>(
      (std::to_underlying(selectedOption) + 1) % optionsSize
    );
  }
  if (IsKeyPressed(KEY_UP)) {
    selectedOption = static_cast<Option>(
      (std::to_underlying(selectedOption) - 1 + optionsSize) % optionsSize
    );
  }
}

bool MainMenu::should_stop_running() const {
  return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE);
}

MainMenu::Option MainMenu::get_selected_option() const {
  return IsKeyDown(KEY_ENTER) ? selectedOption : Option::Exit;
}
