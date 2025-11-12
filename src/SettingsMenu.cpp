#include "SettingsMenu.hpp"
#include "HandlingSettings.hpp"
#include "raylib.h"
#include <algorithm>
#include <format>
#include <fstream>
#include <utility>

SettingsMenu::Config global_config = [] {
  std::ifstream in("settings.raytris");
  if (!in.good())
    return SettingsMenu::Config{Resolution::Small, {20, 1, 30, 15, 7}};

  in.read(reinterpret_cast<char*>(&global_config), sizeof(global_config));
  return global_config;
}();

const SettingsMenu::Config& SettingsMenu::config() {
  return global_config;
}

SettingsMenu::~SettingsMenu() {
  std::ofstream out("settings.raytris");
  out.write(
    reinterpret_cast<const char*>(&global_config), sizeof(global_config)
  );
}

std::pair<int, int> resolution_pair(Resolution resolution) {
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

template <bool HIGHER>
static void resize() {
  constexpr auto RESOLUTIONS = std::to_underlying(Resolution::FullScreen) + 1;
  auto&& resolution = global_config.resolution;
  auto inner = std::to_underlying(resolution);
  if constexpr (HIGHER)
    inner = (inner + 1) % RESOLUTIONS;
  else
    inner = (inner + RESOLUTIONS - 1) % RESOLUTIONS;
  resolution = static_cast<Resolution>(inner);

  const auto [width, height] = resolution_pair(resolution);

#if !defined(PLATFORM_WEB)
  if (IsWindowFullscreen())
    ToggleFullscreen();
  SetWindowSize(width, height);
  if (resolution == Resolution::FullScreen)
    ToggleFullscreen();
#else
  SetWindowSize(width, height);
#endif
}

bool SettingsMenu::should_stop_running() const {
  return IsKeyPressed(KEY_ESCAPE);
}

void SettingsMenu::draw() const {
  const auto [width, height] = resolution_pair(global_config.resolution);
  const float fontSizeBig = height / 4.0;
  const float fontSize = height / 12.0;

  ClearBackground(LIGHTGRAY);
  DrawText(
    "SETTINGS",
    (width - MeasureText("SETTINGS", fontSizeBig)) / 2.0,
    height / 2.0 - fontSize - fontSizeBig,
    fontSizeBig,
    RED
  );

  using option = std::pair<std::string, std::string>;
  option resolution = {"Resolution", std::format("{} x {}", width, height)};
  option das = {
    "Delayed Auto Shift", std::format("{}", global_config.handling_settings.das)
  };
  option softDropFrames = {
    "Soft Drop Frames",
    std::format("{}", global_config.handling_settings.soft_drop)
  };

  std::array options = {resolution, das, softDropFrames};
  for (std::size_t idx = 0; idx < options.size(); idx++) {
    const auto [option, value] = options[idx];
    DrawText(
      option.c_str(),
      width / 8.0f,
      height / 2.0f + idx * fontSize,
      fontSize,
      selected_option == idx ? BLUE : BLACK
    );
    DrawText(
      value.c_str(),
      width / 1.5f,
      height / 2.0f + idx * fontSize,
      fontSize,
      selected_option == idx ? BLUE : BLACK
    );
  }
}

void SettingsMenu::update() {
  auto&& hand_set = global_config.handling_settings;

  if (IsKeyPressed(KEY_UP))
    selected_option = (selected_option + OPTIONS - 1) % OPTIONS;
  if (IsKeyPressed(KEY_DOWN))
    selected_option = (selected_option + 1) % OPTIONS;

  if (selected_option == 0) {
    if (IsKeyPressed(KEY_RIGHT))
      resize<true>();
    if (IsKeyPressed(KEY_LEFT))
      resize<false>();
  } else if (selected_option == 1) {
    if (IsKeyPressed(KEY_LEFT))
      hand_set.das -= 1;
    if (IsKeyPressed(KEY_RIGHT))
      hand_set.das += 1;
    hand_set.das = std::clamp(hand_set.das, 0, 20);
  } else if (selected_option == 2) {
    if (IsKeyPressed(KEY_LEFT))
      hand_set.soft_drop -= 1;
    if (IsKeyPressed(KEY_RIGHT))
      hand_set.soft_drop += 1;
    hand_set.soft_drop = std::clamp(hand_set.soft_drop, 0, 20);
  }
}
