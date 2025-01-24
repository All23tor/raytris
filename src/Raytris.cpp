#include "Raytris.hpp"
#include <raylib.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Raytris::Raytris() {
  Resolution initialResolution = SettingsMenu::getResolution();
  auto [windowWidth, windowHeight] = resolutionPair(initialResolution);
  InitWindow(windowWidth, windowHeight, "RayTris");
  if (initialResolution == Resolution::FullScreen)
    ToggleFullscreen();
}

Raytris::~Raytris() {
  CloseWindow();
}

void Raytris::handleWhereToGo(auto&& runnable) {
  using T = std::decay_t<decltype(runnable)>;
  if constexpr (std::is_same_v<T, Menu>) {
    switch (runnable.getSelectedOption()) {
    case Menu::Option::Exit:
      shouldStopRunning = true;
      break;
    case Menu::Option::SinglePlayer:
      raytris.emplace<SinglePlayerGame>();
      std::get<SinglePlayerGame>(raytris).loadGame();
      break;
    case Menu::Option::TwoPlayers:
      raytris.emplace<TwoPlayerGame>();
      break;
    case Menu::Option::Settings:
      raytris.emplace<SettingsMenu>();
      break;
    }
  } else if constexpr (std::is_same_v<T, SinglePlayerGame>) {
    runnable.saveGame();
    raytris.emplace<Menu>();
  } else {
    raytris.emplace<Menu>();
  }
}

void Raytris::updateDrawFrame() {
  std::visit([this](auto&& app){
    app.update();
    if (app.shouldStopRunning())
      handleWhereToGo(app);
    BeginDrawing();
    ClearBackground(DrawingDetails::BackgroundColor);
    app.draw();
    EndDrawing();
  }, raytris);
}

void Raytris::run() {
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop_arg(
    [](void* p) -> void {((Raytris*)p)->updateDrawFrame();},
    (void*)this, 0, true
  );
#else
  SetTargetFPS(60);
  while (!shouldStopRunning) {
    updateDrawFrame();
  }
#endif
}
