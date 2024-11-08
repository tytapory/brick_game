#include "controller.h"

class GameApplication : public QApplication {
 public:
  GameApplication(int &argc, char **argv)
      : QApplication(argc, argv), timer(new QTimer(this)) {
    Game = new Snake();
    brick_game = new ConsoleWindow();
    brick_game->show();
    holdLeft = false;
    holdRight = false;
    holdUp = false;
    holdDown = false;
    connect(timer, &QTimer::timeout, this, &GameApplication::updateGame);
    connect(brick_game->up_btn, &QPushButton::pressed, this, [this]() {
      Game->userInput(Up);
      holdUp = true;
    });
    connect(brick_game->down_btn, &QPushButton::pressed, this, [this]() {
      Game->userInput(Down);
      holdDown = true;
    });
    connect(brick_game->left_btn, &QPushButton::pressed, this, [this]() {
      Game->userInput(Left);
      holdLeft = true;
    });
    connect(brick_game->right_btn, &QPushButton::pressed, this, [this]() {
      Game->userInput(Right);
      holdRight = true;
    });
    connect(brick_game->up_btn, &QPushButton::released, this, [this]() {
      Game->userInput(Up, false);
      holdUp = false;
    });
    connect(brick_game->down_btn, &QPushButton::released, this, [this]() {
      Game->userInput(Down, false);
      holdDown = false;
    });
    connect(brick_game->left_btn, &QPushButton::released, this, [this]() {
      Game->userInput(Left, false);
      holdLeft = false;
    });
    connect(brick_game->right_btn, &QPushButton::released, this, [this]() {
      Game->userInput(Right, false);
      holdRight = false;
    });

    connect(brick_game->pause_btn, &QPushButton::pressed, this,
            [this]() { Game->userInput(Pause); });
    connect(brick_game->start_btn, &QPushButton::pressed, this,
            [this]() { Game->userInput(Start); });
    connect(brick_game->shutdown_btn, &QPushButton::pressed, this,
            [this]() { this->quit(); });
    timer->start(100);
  }

  ~GameApplication() {
    delete brick_game;
    delete timer;
  }

  void updateGame() {
    if (Game->state.getStatus() == Snake::GameOver) {
      timer->setInterval(5000);  // Остановка таймера, если игра окончена
      Game->updateCurrentState();
      delete Game;
      Game = new Snake();
    } else {
      if (holdUp) Game->userInput(Up, true);
      if (holdDown) Game->userInput(Down, true);
      if (holdLeft) Game->userInput(Left, true);
      if (holdRight) Game->userInput(Right, true);

      GameInfo_t gameinfo = Game->updateCurrentState();
      brick_game->console_display->render(gameinfo);
      timer->setInterval(gameinfo.speed / 500);
    }
  }

 private:
  ConsoleWindow *brick_game;
  Snake *Game;
  QTimer *timer;
  bool holdLeft;
  bool holdRight;
  bool holdUp;
  bool holdDown;
};

int main(int argc, char **argv) {
  GameApplication app(argc, argv);
  return app.exec();
}