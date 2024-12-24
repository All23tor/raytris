#include "Controller.hpp"

Controller::Controller(
  Input restartInput,
  Input swapInput,
  Input leftInput,
  Input rightInput,
  Input leftDasInput,
  Input rightDasInput,
  Input clockwiseInput,
  Input counterClockwiseInput,
  Input oneEightyInput,
  Input hardDropInput,
  Input softDropInput,
  Input undoInput,
  Input pauseInput,
  Input quitInput
): 
  checkRestartInput(restartInput),
  checkSwapInput(swapInput),
  checkLeftInput(leftInput),
  checkRightInput(rightInput),
  checkLeftDasInput(leftDasInput),
  checkRightDasInput(rightDasInput),
  checkClockwiseInput(clockwiseInput),
  checkCounterClockwiseInput(counterClockwiseInput),
  checkOneEightyInput(oneEightyInput),
  checkHardDropInput(hardDropInput),
  checkSoftDropInput(softDropInput),
  checkUndoInput(undoInput),
  checkPauseInput(pauseInput),
  checkQuitInput(quitInput)
{}