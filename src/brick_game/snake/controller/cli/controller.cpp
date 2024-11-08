#include "controller.h"

int main() {
  Snake* Game = new Snake();
  initscr();
  curs_set(0);
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  render(Game->updateCurrentState());
  while (1) {
    if (Game->state.getStatus() == Snake::GameOver) {
      usleep(500000);
      delete Game;
      Game = new Snake();
    } else {
      timeout(0);
      int userInput = getch();
      if (userInput == KEY_LEFT) {
        Game->userInput(Left, Game->state.getDirection() == Left);
      } else if (userInput == KEY_RIGHT) {
        Game->userInput(Right, Game->state.getDirection() == Right);
      } else if (userInput == KEY_DOWN) {
        Game->userInput(Down, Game->state.getDirection() == Down);
      } else if (userInput == KEY_UP) {
        Game->userInput(Up, Game->state.getDirection() == Up);
      } else if (userInput == START_BUTTON) {
        Game->userInput(Start);
      } else if (userInput == PAUSE_BUTTON) {
        Game->userInput(Pause);
      } else if (userInput == TERMINATE_BUTTON) {
        return 0;
      } else {
        Game->userInput(Game->state.getDirection(), false);
      }
      flushinp();
      GameInfo_t gameinfo = Game->updateCurrentState();
      render(gameinfo);
      usleep(gameinfo.speed);
    }
  }
}