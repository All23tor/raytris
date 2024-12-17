#ifndef MENU_H
#define MENU_H

#include <utility>

enum class Resolutions { Small, Medium, Big, FullScreen };

class Menu {
public:
  static constexpr int INITIAL_WIDTH = 640;
  static constexpr int INITIAL_HEIGHT = 360;

private:
  Resolutions resolution = Resolutions::Small;
  std::pair<int, int> windowResolution;

  void draw() const;
  void update();
  void resizeScreen();

public:
  Menu();
  enum class ExitCode { Game, Exit };
  ExitCode run();
  const std::pair<int, int> getResolution(Resolutions resolution) const;
};

#endif
