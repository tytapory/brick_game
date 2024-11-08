#include "snake_adapter.h"
#include "stdio.h"
#include "../../brick_game/snake/logic/snake.h"
extern "C" {
    Snake* Snake_new() {
        return new Snake();
    }

    void Snake_delete(Snake* s) {
        delete s;
    }

    GameInfo_t Snake_updateCurrentState(Snake* s) {
        if (s == NULL) {
            printf("snake null\n");
        }
        printf("score %d\n", s->state.getScore());
        printf("high score %d\n", s->state.getHighScore());
        return s->updateCurrentState();
    }

    int getStatus(Snake* s) {
        return s->state.getStatus();
    }

    void Snake_userInput(Snake* s, int action, bool hold) {
        s->userInput(action, hold);
    }
}

