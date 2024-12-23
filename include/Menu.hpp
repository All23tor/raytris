#ifndef MENU_H
#define MENU_H

#include <utility>

enum class Resolutions { Small, Medium, Big, FullScreen };

class Menu {
public:
  static constexpr int InitialWidth = 640;
  static constexpr int InitialHeight = 360;

private:
  Resolutions resolution = Resolutions::Small;
  std::pair<int, int> windowResolution;

  void draw() const;
  void update();
  void resizeScreen();

public:
  Menu();
  enum class ExitCode { SinglePlayerGame, TwoPlayerGame, Exit };
  ExitCode run();
  const std::pair<int, int> getResolution(Resolutions resolution) const;
};

#endif
