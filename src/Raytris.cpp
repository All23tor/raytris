#include "Raytris.hpp"
#include "SettingsMenu.hpp"
#include <raylib.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Raytris::Raytris() {
  Resolution resolution = SettingsMenu::config().resolution;
  auto [width, height] = resolution_pair(resolution);
  InitWindow(width, height, "RayTris");
#if !defined(PLATFORM_WEB)
  if (resolution == Resolution::FullScreen)
    ToggleFullscreen();
#endif
}

Raytris::~Raytris() {
  CloseWindow();
}

void Raytris::handle_stop_runnig(auto&& runnable) {
  using T = std::decay_t<decltype(runnable)>;
  if constexpr (std::is_same_v<T, MainMenu>) {
    auto&& handling_settings = SettingsMenu::config().handling_settings;
    switch (runnable.get_selected_option()) {
    case MainMenu::Option::Exit:
      should_stop_running = true;
      break;
    case MainMenu::Option::SinglePlayer:
      raytris.emplace<SinglePlayerGame>(handling_settings);
      break;
    case MainMenu::Option::TwoPlayers:
      raytris.emplace<TwoPlayerGame>(handling_settings, handling_settings);
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
