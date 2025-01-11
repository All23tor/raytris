#include "Raytris.hpp"
#include <functional>
#include <raylib.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Raytris::Raytris() {
  InitWindow(Menu::InitialWidth, Menu::InitialHeight, "RayTris");
}

Raytris::~Raytris() {
  CloseWindow();
}

void Raytris::handleWhereToGo(auto&& runnable) {
  using T = std::decay_t<decltype(runnable)>;
  if constexpr (std::is_same_v<T, Menu*>) {
    switch (runnable->getSelectedOption()) {
    case Menu::Option::Exit:
      shouldStopRunning = true;
      break;
    case Menu::Option::SinglePlayer:
      raytris.emplace<SinglePlayerGame>();
      break;
    case Menu::Option::TwoPlayers:
      raytris.emplace<TwoPlayerGame>();
      break;
    }
  } else {
    raytris.emplace<Menu*>(&menu);
  }
}

void Raytris::updateDrawFrame() {
  std::visit(&Updateable::update, raytris);

  if (std::visit(&Runnable::shouldStopRunning, raytris)) {
    std::visit([&](auto& runnable) {this->handleWhereToGo(runnable);}, raytris);
  }

  BeginDrawing();
  ClearBackground(DrawingDetails::BackgroundColor);
  std::visit(&Drawable::draw, raytris);
  EndDrawing();
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
