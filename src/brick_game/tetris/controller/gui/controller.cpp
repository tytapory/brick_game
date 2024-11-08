#include "controller.h"

class GameApplication : public QApplication {
 public:
  GameApplication(int &argc, char **argv)
      : QApplication(argc, argv), timer(new QTimer(this)) {
    brick_game = new ConsoleWindow();
    brick_game->show();
    connect(timer, &QTimer::timeout, this, &GameApplication::updateGame);
    connect(brick_game->down_btn, &QPushButton::pressed, this, [this]() {
      if (get_current_state()->pause == None) {
        while (block_is_attached() == 0) {
          (get_current_state()->coord_x)++;
          brick_game->console_display->render(updateCurrentState());
          usleep(10000);
        }
        userInput(Down);
        brick_game->console_display->render(updateCurrentState());
      }
    });
    connect(brick_game->left_btn, &QPushButton::pressed, this, [this]() {
      if (get_current_state()->pause == None) {
        userInput(Left);
        brick_game->console_display->render(updateCurrentState());
      }
    });
    connect(brick_game->right_btn, &QPushButton::pressed, this, [this]() {
      if (get_current_state()->pause == None) {
        userInput(Right);
        brick_game->console_display->render(updateCurrentState());
      }
    });
    connect(brick_game->big_btn, &QPushButton::pressed, this, [this]() {
      if (get_current_state()->pause == None) {
        userInput(Action);
        brick_game->console_display->render(updateCurrentState());
      }
    });

    connect(brick_game->pause_btn, &QPushButton::pressed, this, [this]() {
      userInput(Pause);
      brick_game->console_display->render(updateCurrentState());
    });
    connect(brick_game->start_btn, &QPushButton::pressed, this,
            [this]() { userInput(Start); });
    connect(brick_game->shutdown_btn, &QPushButton::pressed, this,
            [this]() { this->quit(); });
    initialize_state();
    timer->start(100);
    brick_game->console_display->render(updateCurrentState());
  }

  ~GameApplication() {
    delete brick_game;
    delete timer;
  }

  void updateGame() {
    State_t *state = get_current_state();
    if (state->pause == None) {
      if (state->status == Initial) {
        state->status = Spawn;
      } else if (state->status == Spawn) {
        spawn_new_block();
      } else if (state->status == Shifting || state->status == Moving) {
        state->status = Shifting;
        shift_block();
      } else if (state->status == Attaching) {
        attach_block();
      } else if (state->status == GameOver) {
        finish_game();
      }
      GameInfo_t info = updateCurrentState();
      brick_game->console_display->render(info);
      timer->setInterval(info.speed);
    }
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