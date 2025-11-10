#include "Raytris.hpp"
#include "SettingsMenu.hpp"
#include <raylib.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Raytris::Raytris() {
  Resolution initial_resolution = SettingsMenu::getResolution();
  auto [width, height] = resolutionPair(initial_resolution);
  InitWindow(width, height, "RayTris");
  if (initial_resolution == Resolution::FullScreen)
    ToggleFullscreen();
}

Raytris::~Raytris() {
  CloseWindow();
}

void Raytris::handle_stop_runnig(auto&& runnable) {
  using T = std::decay_t<decltype(runnable)>;
  if constexpr (std::is_same_v<T, MainMenu>) {
    switch (runnable.get_selected_option()) {
    case MainMenu::Option::Exit:
      should_stop_running = true;
      break;
    case MainMenu::Option::SinglePlayer:
      raytris.emplace<SinglePlayerGame>(SettingsMenu::getHandlingSettings());
      break;
    case MainMenu::Option::TwoPlayers:
      raytris.emplace<TwoPlayerGame>(
        SettingsMenu::getHandlingSettings(), SettingsMenu::getHandlingSettings()
      );
      break;
    case MainMenu::Option::Settings:
      raytris.emplace<SettingsMenu>();
      break;
    }
  } else {
    raytris.emplace<MainMenu>();
  }
}

void Raytris::run() {
  static constexpr auto update_draw = [](Raytris* self) {
    std::visit(
      [self](auto&& app) {
        app.update();
        if (app.should_stop_running())
          self->handle_stop_runnig(app);
        BeginDrawing();
        ClearBackground(DrawingDetails::BACKGROUND_COLOR);
        app.draw();
        EndDrawing();
      },
      self->raytris
    );
  };

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop_arg(
    [](void* p) -> void { update_draw((Raytris*)p); }, (void*)this, 0, true
  );
#else
  SetTargetFPS(60);
  while (!should_stop_running) {
    update_draw(this);
  }
#endif
}
