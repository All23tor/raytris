#include <emscripten/emscripten.h>
#include "SinglePlayerGame.hpp"
#include "TwoPlayerGame.hpp"
#include "Menu.hpp"
#include <variant>

namespace RaylibWeb {
  namespace {
    Menu menu;
    std::variant<Menu*, SinglePlayerGame, TwoPlayerGame> raytris = &menu;

    auto handleWhereToGo = [](auto&& runnable) -> void {
      using T = std::decay_t<decltype(runnable)>;
      if constexpr (std::is_same_v<T, Menu*>) {
        switch (runnable->getSelectedOption()) {
        case Menu::Option::Exit: return;
        case Menu::Option::SinglePlayer:
          raytris.emplace<SinglePlayerGame>();
          break;
        case Menu::Option::TwoPlayers:
          raytris.emplace<TwoPlayerGame>();
          break;
        }
      } else if constexpr (std::is_same_v<T, SinglePlayerGame>) {
        raytris.emplace<Menu*>(&menu);
      } else if constexpr (std::is_same_v<T, TwoPlayerGame>) {
        raytris.emplace<Menu*>(&menu);
      } else {
        static_assert(false, "non-exhaustive visitor!");
      }
    };
  }

  void UpdateDrawFrame() {
    std::visit(&Updateable::update, raytris);

    if (std::visit(&Runnable::shouldStopRunning, raytris)) {
      std::visit(handleWhereToGo, raytris);
    }

    BeginDrawing();
    ClearBackground(DrawingDetails::BackgroundColor);
    std::visit(&Drawable::draw, raytris);
    EndDrawing();
  }
}


int main() {
  InitWindow(Menu::InitialWidth, Menu::InitialHeight, "Raytris");
  emscripten_set_main_loop(RaylibWeb::UpdateDrawFrame, 0, 1);
  CloseWindow();
}

