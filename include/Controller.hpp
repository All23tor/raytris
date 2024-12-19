#include <functional>

struct Controller {
  const std::function<bool()> checkRestartInput;
  const std::function<bool()> checkSwapInput;
  const std::function<bool()> checkLeftInput;
  const std::function<bool()> checkRightInput;
  const std::function<bool()> checkLeftDasInput;
  const std::function<bool()> checkRightDasInput;
  const std::function<bool()> checkClockwiseInput;
  const std::function<bool()> checkCounterClockwiseInput;
  const std::function<bool()> checkOneEightyInput;
  const std::function<bool()> checkHardDropInput;
  const std::function<bool()> checkSoftDropInput;
  const std::function<bool()> checkUndoInput;
  const std::function<bool()> checkPauseInput;
  const std::function<bool()> checkQuitInput;
};