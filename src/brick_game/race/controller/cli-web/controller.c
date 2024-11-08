#include "controller.h"

void initialize() {
	initscr();
  curs_set(0);
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
}

void renderScr(GameInfo_t info) {
  render(info);
}

int getInput() {
  timeout(0);
  int userInput = getch();
  return userInput;
  flushinp();
}
