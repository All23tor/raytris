#include "MainMenu.hpp"
#include "raylib.h"
#include <string>
#include <utility>
#include <array>

namespace {
  const char * optionToString(MainMenu::Option option) {
    switch (option) {
      case MainMenu::Option::SinglePlayer: return "Single Player";
      case MainMenu::Option::TwoPlayers: return "Two Players";
      case MainMenu::Option::Settings: return "Settings";
      case MainMenu::Option::Exit: return "Exit";
    }
    return "";
  }
};

namespace {
  struct Button {
    Rectangle rec;
    bool isClicked() {
      Vector2 mousePosition = GetMousePosition();
      return CheckCollisionPointRec(mousePosition, rec);
    }

    void draw() const {
      
    }
  };
};

void MainMenu::draw() const {
  const int windowWidth = GetScreenWidth();
  const int windowHeight = GetScreenHeight();
  const float fontSize = windowHeight / 12.0;
  const float fontSizeBig = windowHeight / 5.0;

  ClearBackground(LIGHTGRAY);
  DrawText("RAYTRIS",
    (windowWidth - MeasureText("RAYTRIS", fontSizeBig)) / 2.0,
    windowHeight / 2.0 - fontSizeBig - fontSize, fontSizeBig, RED);
  
  constexpr auto options = [](){
    std::array<Option, std::to_underlying(Option::Exit)> res;
    for (int i=0; i<res.size(); i++) {
      res[i] = static_cast<Option>(i);
    }
    return res;
  }();
  for (int i = 0; i < options.size(); i++) {
    const auto s = optionToString(options[i]);
    const bool isSelected = options[i] == selectedOption;
    const float boxWidth = 8.0f * fontSize;
    const float separation = 1.5 * fontSize;
    const float boxHeight = 1.3f * fontSize;
    const Rectangle box = {(windowWidth - boxWidth) / 2.0f, (windowHeight - boxHeight + fontSize) / 2.0f + i * separation, boxWidth, boxHeight};
    DrawRectangleRec(box, isSelected ? SKYBLUE : GRAY);
    DrawRectangleLinesEx(box, fontSize / 10.0, isSelected ? BLUE : BLACK);
    DrawText(s, (windowWidth  - MeasureText(s, fontSize)) / 2.0, windowHeight / 2.0 + i * separation, fontSize, isSelected ? BLUE : BLACK);
  }

}

void MainMenu::update() {
  constexpr auto optionsSize = std::to_underlying(Option::Exit); 
  if (IsKeyPressed(KEY_DOWN)) {
    selectedOption = static_cast<Option>((std::to_underlying(selectedOption) + 1) % optionsSize);
  }
  if (IsKeyPressed(KEY_UP)) {
    selectedOption = static_cast<Option>((std::to_underlying(selectedOption) - 1 + optionsSize) % optionsSize);
  }
}

bool MainMenu::shouldStopRunning() const {
  return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE);
}

MainMenu::Option MainMenu::getSelectedOption() const {
  return IsKeyDown(KEY_ENTER) ? selectedOption : Option::Exit;
}
