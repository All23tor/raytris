#include "SettingsMenu.hpp"
#include "HandlingSettings.hpp"
#include "raylib.h"
#include <format>
#include <fstream>
#include <utility>

// TODO: Change this initialization aberration
Resolution SettingsMenu::resolution = [] {
  std::ifstream in("settings.raytris");
  if (!in.good())
    return Resolution::Small;

  Resolution resolution;
  in.read(reinterpret_cast<char*>(&resolution), sizeof(resolution));
  in.read(
    reinterpret_cast<char*>(&handling_settings), sizeof(handling_settings)
  );
  return resolution;
}();

SettingsMenu::~SettingsMenu() {
  std::ofstream out("settings.raytris");
  out.write(reinterpret_cast<const char*>(&resolution), sizeof(resolution));
  out.write(
    reinterpret_cast<const char*>(&handling_settings), sizeof(handling_settings)
  );
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
  resolution = static_cast<Resolution>(
    (std::to_underlying(resolution) + 1) %
    (std::to_underlying(Resolution::FullScreen) + 1)
  );
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
  resolution = static_cast<Resolution>(
    (std::to_underlying(resolution) +
     std::to_underlying(Resolution::FullScreen)) %
    (std::to_underlying(Resolution::FullScreen) + 1)
  );
  const auto [windowWidth, windowHeight] = resolutionPair(resolution);

  if (IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  SetWindowSize(windowWidth, windowHeight);

  if (resolution == Resolution::FullScreen) {
    ToggleFullscreen();
  }
}

bool SettingsMenu::should_stop_running() const {
  return IsKeyPressed(KEY_ESCAPE);
}

void SettingsMenu::draw() const {
  const auto [windowWidth, windowHeight] = resolutionPair(resolution);
  const float fontSizeBig = windowHeight / 4.0;
  const float fontSize = windowHeight / 12.0;

  ClearBackground(LIGHTGRAY);
  DrawText(
    "SETTINGS",
    (windowWidth - MeasureText("SETTINGS", fontSizeBig)) / 2.0,
    windowHeight / 2.0 - fontSize - fontSizeBig,
    fontSizeBig,
    RED
  );

  using option = std::pair<std::string, std::string>;
  option resolution = {
    "Resolution", std::format("{} x {}", windowWidth, windowHeight)
  };
  option das = {"Delayed Auto Shift", std::format("{}", handling_settings.das)};
  option softDropFrames = {
    "Soft Drop Frames", std::format("{}", handling_settings.softDropFrames)
  };

  std::array options = {resolution, das, softDropFrames};
  for (std::size_t idx = 0; idx < options.size(); idx++) {
    const auto [option, value] = options[idx];
    DrawText(
      option.c_str(),
      windowWidth / 8.0f,
      windowHeight / 2.0f + idx * fontSize,
      fontSize,
      selected_option == idx ? BLUE : BLACK
    );
    DrawText(
      value.c_str(),
      windowWidth / 1.5f,
      windowHeight / 2.0f + idx * fontSize,
      fontSize,
      selected_option == idx ? BLUE : BLACK
    );
  }
}

void SettingsMenu::update() {
  if (IsKeyPressed(KEY_UP))
    selected_option = (selected_option + 2) % 3;
  if (IsKeyPressed(KEY_DOWN))
    selected_option = (selected_option + 1) % 3;

  if (selected_option == 0) {
    if (IsKeyPressed(KEY_RIGHT))
      resizeScreenHigher();
    if (IsKeyPressed(KEY_LEFT))
      resizeScreenLower();
  } else if (selected_option == 1) {
    if (IsKeyPressed(KEY_LEFT) && handling_settings.das > 0)
      handling_settings.das -= 1;
    if (IsKeyPressed(KEY_RIGHT) && handling_settings.das < 20)
      handling_settings.das += 1;
  } else if (selected_option == 2) {
    if (IsKeyPressed(KEY_LEFT) && handling_settings.softDropFrames > 0)
      handling_settings.softDropFrames -= 1;
    if (IsKeyPressed(KEY_RIGHT) && handling_settings.softDropFrames < 20)
      handling_settings.softDropFrames += 1;
  }
}

Resolution SettingsMenu::getResolution() {
  return resolution;
}

const HandlingSettings& SettingsMenu::getHandlingSettings() {
  return handling_settings;
}
