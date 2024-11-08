#include "controller.h"

class GameApplication : public QApplication {
 public:
  GameApplication(int &argc, char **argv)
      : QApplication(argc, argv), timer(new QTimer(this)) {
    StartGame();
    brick_game = new ConsoleWindow();
    brick_game->show();
    holdLeft = false;
    holdRight = false;
    holdUp = false;
    holdDown = false;
    connect(timer, &QTimer::timeout, this, &GameApplication::updateGame);
    connect(brick_game->up_btn, &QPushButton::pressed, this, [this]() {
      HandleUserInput(Up);
    });
    connect(brick_game->down_btn, &QPushButton::pressed, this, [this]() {
      HandleUserInput(Down);
    });
    connect(brick_game->left_btn, &QPushButton::pressed, this, [this]() {
      HandleUserInput(Left);
    });
    connect(brick_game->right_btn, &QPushButton::pressed, this, [this]() {
      HandleUserInput(Right);
    });
   
    connect(brick_game->pause_btn, &QPushButton::pressed, this,
            [this]() { HandleUserInput(Pause); });
    connect(brick_game->start_btn, &QPushButton::pressed, this,
            [this]() { HandleUserInput(Start); });
    connect(brick_game->shutdown_btn, &QPushButton::pressed, this,
            [this]() { this->quit(); });
    timer->start(100);
  }

  ~GameApplication() {
    delete brick_game;
    delete timer;
  }

  void updateGame() {
    GameInfo_t gameinfo = UpdateGame();
    brick_game->console_display->render(gameinfo);
    timer->setInterval(gameinfo.speed);
  }

 private:
  ConsoleWindow *brick_game;
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
