#include "SettingsMenu.hpp"
#include "HandlingSettings.hpp"
#include "raylib.h"
#include <fstream>
#include <format>
#include <utility>

HandlingSettings SettingsMenu::handlingSettings;
Resolution SettingsMenu::resolution = []{
  std::ifstream in("settings.raytris");
  if (!in.good()) {
    handlingSettings = {20, 1, 30, 15, 7};
    return Resolution::Small;
  }
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
    return { 640, 360 };
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

void SettingsMenu::resizeScreenHigher() {
  resolution = static_cast<Resolution>((std::to_underlying(resolution) + 1) % (std::to_underlying(Resolution::FullScreen) + 1));
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
  resolution = static_cast<Resolution>((std::to_underlying(resolution) + std::to_underlying(Resolution::FullScreen)) % (std::to_underlying(Resolution::FullScreen) + 1));
  const auto [windowWidth, windowHeight] = resolutionPair(resolution);

  if (IsWindowFullscreen()) {
    ToggleFullscreen();
  }

  SetWindowSize(windowWidth, windowHeight);

  if (resolution == Resolution::FullScreen) {
    ToggleFullscreen();
  }
}

bool SettingsMenu::shouldStopRunning() const  {
  return IsKeyPressed(KEY_ESCAPE);
}

void SettingsMenu::draw() const {
  const auto [windowWidth, windowHeight] = resolutionPair(resolution);
  const float fontSize = windowHeight / 10.0;

  ClearBackground(LIGHTGRAY);
  DrawText("SETTINGS",
    (windowWidth - MeasureText("SETTINGS", fontSize * 2)) / 2.0,
    windowHeight / 2.0 - 3 * fontSize, fontSize * 2, RED);

  std::string resolution = std::format("{} x {}", windowWidth, windowHeight);
  DrawText(resolution.c_str(),
    (windowWidth - MeasureText(resolution.c_str(), fontSize)) / 2.0,
    windowHeight / 2.0 - 1 * fontSize, fontSize, option == 0 ? BLUE : BLACK);

  std::string das = std::format("DAS: {}", handlingSettings.das);
  DrawText(das.c_str(),
    (windowWidth - MeasureText(das.c_str(), fontSize)) / 2.0,
    windowHeight / 2.0 + 0 * fontSize, fontSize, option == 1 ? BLUE : BLACK);
  
  std::string softDropFrames = std::format("Soft Drop Frames: {}", handlingSettings.softDropFrames);
  DrawText(softDropFrames.c_str(),
    (windowWidth - MeasureText(softDropFrames.c_str(), fontSize)) / 2.0,
    windowHeight / 2.0 + 1 * fontSize, fontSize, option == 2 ? BLUE : BLACK); 
}

void SettingsMenu::update() {
  if (IsKeyPressed(KEY_UP))
    option = (option + 2) % 3;
  if (IsKeyPressed(KEY_DOWN))
    option = (option + 1) % 3;


  if (option == 0) {
    if (IsKeyPressed(KEY_RIGHT))
      resizeScreenHigher();
    if (IsKeyPressed(KEY_LEFT))
      resizeScreenLower();
  } else if (option == 1) { 
    if (IsKeyPressed(KEY_LEFT) && handlingSettings.das > 0)
      handlingSettings.das -= 1;
    if (IsKeyPressed(KEY_RIGHT) && handlingSettings.das < 20)
      handlingSettings.das += 1;
  } else if (option == 2) { 
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
