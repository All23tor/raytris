#include <emscripten/emscripten.h>
#include "SinglePlayerGame.hpp"
#include "Menu.hpp"
#include <variant>

namespace RaylibWeb {
  constexpr int windowWidth = 800, windowHeight = 450;

  namespace {
    std::variant<Menu, SinglePlayerGame> raytris = Menu();

    auto handleWhereToGo = [](auto&& runnable) -> void {
      using T = std::decay_t<decltype(runnable)>;
      if constexpr (std::is_same_v<T, Menu>) {
        switch (runnable.getSelectedOption()) {
        case Menu::Option::Exit: return;
        case Menu::Option::SinglePlayer: {
          raytris.emplace<SinglePlayerGame>(DrawingDetails{
            DrawingDetails::HeightScaleFactor * windowHeight / (Playfield::VisibleHeight),
            { windowWidth / 2.0f, windowHeight / 2.0f }
            });
          return;
        }
        case Menu::Option::TwoPlayers: return;
        }
      } else if constexpr (std::is_same_v<T, SinglePlayerGame>) {
        raytris.emplace<Menu>();
      } else {
        static_assert(false, "non-exhaustive visitor!");
      }
    };
  }

  void UpdateDrawFrame() {
    std::visit(&Updateable::update, raytris);

    if (!std::visit(&Runnable::shouldStopRunning, raytris)) {
      std::visit(handleWhereToGo, raytris);
    }

    BeginDrawing();
    ClearBackground(DrawingDetails::BackgroundColor);
    std::visit(&Drawable::draw, raytris);
    EndDrawing();
  }
}


int main() {
  InitWindow(RaylibWeb::windowWidth, RaylibWeb::windowHeight, "Raytris");
  emscripten_set_main_loop(RaylibWeb::UpdateDrawFrame, 0, 1);
  CloseWindow();
}

