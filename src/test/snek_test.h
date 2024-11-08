#ifndef SNEK_TEST_H
#define SNEK_TEST_H

#include <gtest/gtest.h>

#include "../brick_game/snake/logic/snake.h"

void destroy_apple(Snake& Game);
std::pair<int, int> getHeadCoordinates(Snake& Game);
void spawnAppleAt(Snake& Game, int x, int y);
#endif  // SNEK_TEST_H