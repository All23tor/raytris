#include "SettingsMenu.hpp"
#include "HandlingSettings.hpp"
#include "raylib.h"
#include <format>
#include <fstream>
#include <utility>

namespace {
std::ostream& operator<<(std::ostream& o, const HandlingSettings& settings) {
  o << settings.gravityFrames << ' ';
  o << settings.softDropFrames << ' ';
  o << settings.maxLockDelayFrames << ' ';
  o << settings.maxLockDelayResets << ' ';
  o << settings.das << '\n';
  return o;
}

std::istream& operator>>(std::istream& i, HandlingSettings& settings) {
  i >> settings.gravityFrames;
  i >> settings.softDropFrames;
  i >> settings.maxLockDelayFrames;
  i >> settings.maxLockDelayResets;
  i >> settings.das;
  return i;
}
} // namespace

// TODO: Change this initialization aberration
Resolution SettingsMenu::resolution = [] {
  std::ifstream in("settings.raytris");
  if (!in.good())
    return Resolution::Small;

  int input_resolution;
  in >> input_resolution;
  in >> handlingSettings;
  return static_cast<Resolution>(input_resolution);
}();

SettingsMenu::~SettingsMenu() {
  std::ofstream out("settings.raytris");
  out << std::to_underlying(resolution) << '\n';
  out << handlingSettings << '\n';
}

std::pair<int, int> resolutionPair(Resolution resolution) {
  switch (resolution) {
  case Resolution::Small:
    return {640, 360};
  case Resolution::Medium:
    return {960, 540};
  case Resolution::Big:
    return {1280, 720};
  case Resolution::FullScreen: {
    int monitor = GetCurrentMonitor();
    return {GetMonitorWidth(monitor), GetMonitorHeight(monitor)};
  }
  default:
    return {0, 0};
  }
}

void SettingsMenu::resizeScreenHigher() {
  resolution =
    static_cast<Resolution>((std::to_underlying(resolution) + 1) %
                            (std::to_underlying(Resolution::FullScreen) + 1));
  const auto [windowWidth, windowHeight] = resolutionPair(resolution);

  if (IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  SetWindowSize(windowWidth, windowHeight);

  if (resolution == Resolution::FullScreen) {
    ToggleFullscreen();
  }
}

void SettingsMenu::resizeScreenLower() {
  resolution =
    static_cast<Resolution>((std::to_underlying(resolution) +
                             std::to_underlying(Resolution::FullScreen)) %
                            (std::to_underlying(Resolution::FullScreen) + 1));
  const auto [windowWidth, windowHeight] = resolutionPair(resolution);

  if (IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  SetWindowSize(windowWidth, windowHeight);

  if (resolution == Resolution::FullScreen) {
    ToggleFullscreen();
  }
}

bool SettingsMenu::shouldStopRunning() const {
  return IsKeyPressed(KEY_ESCAPE);
}

void SettingsMenu::draw() const {
  const auto [windowWidth, windowHeight] = resolutionPair(resolution);
  const float fontSizeBig = windowHeight / 4.0;
  const float fontSize = windowHeight / 12.0;

  ClearBackground(LIGHTGRAY);
  DrawText("SETTINGS",
           (windowWidth - MeasureText("SETTINGS", fontSizeBig)) / 2.0,
           windowHeight / 2.0 - fontSize - fontSizeBig, fontSizeBig, RED);

  using option = std::pair<std::string, std::string>;
  option resolution = {"Resolution",
                       std::format("{} x {}", windowWidth, windowHeight)};
  option das = {"Delayed Auto Shift", std::format("{}", handlingSettings.das)};
  option softDropFrames = {"Soft Drop Frames",
                           std::format("{}", handlingSettings.softDropFrames)};

  std::array options = {resolution, das, softDropFrames};
  for (std::size_t idx = 0; idx < options.size(); idx++) {
    const auto [option, value] = options[idx];
    DrawText(option.c_str(), windowWidth / 8.0f,
             windowHeight / 2.0f + idx * fontSize, fontSize,
             selectedOption == idx ? BLUE : BLACK);
    DrawText(value.c_str(), windowWidth / 1.5f,
             windowHeight / 2.0f + idx * fontSize, fontSize,
             selectedOption == idx ? BLUE : BLACK);
  }
}

void SettingsMenu::update() {
  if (IsKeyPressed(KEY_UP))
    selectedOption = (selectedOption + 2) % 3;
  if (IsKeyPressed(KEY_DOWN))
    selectedOption = (selectedOption + 1) % 3;

  if (selectedOption == 0) {
    if (IsKeyPressed(KEY_RIGHT))
      resizeScreenHigher();
    if (IsKeyPressed(KEY_LEFT))
      resizeScreenLower();
  } else if (selectedOption == 1) {
    if (IsKeyPressed(KEY_LEFT) && handlingSettings.das > 0)
      handlingSettings.das -= 1;
    if (IsKeyPressed(KEY_RIGHT) && handlingSettings.das < 20)
      handlingSettings.das += 1;
  } else if (selectedOption == 2) {
    if (IsKeyPressed(KEY_LEFT) && handlingSettings.softDropFrames > 0)
      handlingSettings.softDropFrames -= 1;
    if (IsKeyPressed(KEY_RIGHT) && handlingSettings.softDropFrames < 20)
      handlingSettings.softDropFrames += 1;
  }
}

Resolution SettingsMenu::getResolution() {
  return resolution;
}

const HandlingSettings& SettingsMenu::getHandlingSettings() {
  return handlingSettings;
}
