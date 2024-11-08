#include "snek_test.h"

class SnekTests : public testing::Test {};

TEST(SnekTests, Initialization) {
  Snake Game;
  EXPECT_TRUE(Game.state.getArrayPtr() != nullptr);
  EXPECT_EQ(Game.state.getStatus(), Snake::Start);
  EXPECT_EQ(Game.state.getScore(), 0);
  EXPECT_EQ(Game.state.getLevel(), 1);
  EXPECT_TRUE(Game.state.isPaused());
  EXPECT_EQ(Game.state.getSpeedMod(), 1);
  int apples = 0;
  bool isThereOneApple = false;
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (Game.state.getArrayPtr()[i][j] == -1) {
        apples++;
      }
    }
  }
  isThereOneApple = apples == 1;
  EXPECT_TRUE(isThereOneApple);
}

TEST(SnekTests, BasicControls) {
  Snake Game;
  GameInfo_t info;
  destroy_apple(Game);
  Game.userInput(Start);
  std::pair headCoordinates = getHeadCoordinates(Game);
  info = Game.updateCurrentState();
  std::pair newHeadCoordinates = getHeadCoordinates(Game);
  EXPECT_EQ(headCoordinates.first + 1, newHeadCoordinates.first);
  EXPECT_EQ(headCoordinates.second, newHeadCoordinates.second);
  Game.userInput(Up);
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  headCoordinates = newHeadCoordinates;
  newHeadCoordinates = getHeadCoordinates(Game);
  EXPECT_EQ(headCoordinates.first, newHeadCoordinates.first);
  EXPECT_EQ(headCoordinates.second - 1, newHeadCoordinates.second);
  EXPECT_EQ(Game.state.getDirection(), Up);
  Game.userInput(Left);
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  headCoordinates = newHeadCoordinates;
  newHeadCoordinates = getHeadCoordinates(Game);
  EXPECT_EQ(headCoordinates.first - 1, newHeadCoordinates.first);
  EXPECT_EQ(headCoordinates.second, newHeadCoordinates.second);
  EXPECT_EQ(Game.state.getDirection(), Left);
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  Game.userInput(Down);
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  headCoordinates = newHeadCoordinates;
  newHeadCoordinates = getHeadCoordinates(Game);
  EXPECT_EQ(headCoordinates.first - 1, newHeadCoordinates.first);
  EXPECT_EQ(headCoordinates.second + 1, newHeadCoordinates.second);
  EXPECT_EQ(Game.state.getDirection(), Down);
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
}

TEST(SnekTests, AdvancedControls) {
  Snake Game;
  GameInfo_t info;
  destroy_apple(Game);
  Game.userInput(Start);
  info = Game.updateCurrentState();
  Game.userInput(Pause);
  EXPECT_TRUE(Game.state.isPaused());
  Game.userInput(Pause);
  EXPECT_FALSE(Game.state.isPaused());
  Game.userInput(Left);  // we can't turn to the opposite direction
  EXPECT_EQ(Game.state.getDirection(), Right);
  Game.userInput(Right, true);
  EXPECT_FALSE(Game.state.getSpeedMod() == 1);
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
}

TEST(SnekTests, EatApple) {
  Snake Game;
  GameInfo_t info;
  destroy_apple(Game);
  Game.userInput(Start);
  std::pair<int, int> headCoords = getHeadCoordinates(Game);
  spawnAppleAt(Game, headCoords.first + 1, headCoords.second);
  info = Game.updateCurrentState();
  EXPECT_EQ(Game.state.getArrayPtr()[headCoords.second][headCoords.first - 3],
            4);
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
}

TEST(SnekTests, DieWall) {
  Snake Game;
  GameInfo_t info;
  destroy_apple(Game);
  Game.userInput(Start);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  EXPECT_EQ(Game.state.getStatus(), Snake::GameOver);
}

TEST(SnekTests, DieSnek) {
  Snake Game;
  GameInfo_t info;
  destroy_apple(Game);
  Game.userInput(Start);
  std::pair<int, int> headCoords = getHeadCoordinates(Game);
  spawnAppleAt(Game, headCoords.first, headCoords.second - 1);
  Game.userInput(Up);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  destroy_apple(Game);
  Game.userInput(Left);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  Game.userInput(Down);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  EXPECT_EQ(Game.state.getStatus(), Snake::GameOver);
}

TEST(SnekTests, LevelUp) {
  Snake Game;
  GameInfo_t info;
  destroy_apple(Game);
  Game.userInput(Start);
  Game.userInput(Up);
  std::pair<int, int> headCoords = getHeadCoordinates(Game);
  spawnAppleAt(Game, headCoords.first, headCoords.second - 1);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  headCoords = getHeadCoordinates(Game);
  spawnAppleAt(Game, headCoords.first, headCoords.second - 1);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  headCoords = getHeadCoordinates(Game);
  spawnAppleAt(Game, headCoords.first, headCoords.second - 1);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  headCoords = getHeadCoordinates(Game);
  spawnAppleAt(Game, headCoords.first, headCoords.second - 1);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  headCoords = getHeadCoordinates(Game);
  spawnAppleAt(Game, headCoords.first, headCoords.second - 1);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  EXPECT_EQ(Game.state.getLevel(), 2);
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  Game.state.addLevel();
  EXPECT_EQ(Game.state.getLevel(), 10);
}

TEST(SnekTests, Win) {
  Snake Game;
  GameInfo_t info;
  Game.userInput(Start);
  destroy_apple(Game);
  Game.state.setScore(196);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  bool isShownWin = true;
  int win[5][10] = {{1, 0, 0, 1, 0, 1, 0, 0, 0, 0},
                    {1, 0, 0, 1, 0, 0, 0, 1, 0, 0},
                    {1, 0, 1, 1, 0, 1, 0, 1, 1, 0},
                    {1, 0, 1, 1, 0, 1, 0, 1, 0, 1},
                    {0, 1, 1, 1, 0, 1, 0, 1, 0, 1}};
  for (int i = 0; i < 5 && isShownWin == true; i++) {
    for (int j = 0; j < 10 && isShownWin == true; j++) {
      isShownWin = Game.state.getArrayPtr()[7 + i][j] == win[i][j];
    }
  }
  EXPECT_TRUE(isShownWin);
}

TEST(SnekTests, HiScore) {
  Snake Game;
  GameInfo_t info;
  Game.userInput(Start);
  destroy_apple(Game);
  Game.state.setScore(2000);
  info = Game.updateCurrentState();
  free_matrix(info.field, FIELD_H);
  free_matrix(info.next, 4);
  EXPECT_EQ(Game.state.getHighScore(), 2000);
}

void destroy_apple(Snake& Game) {
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (Game.state.getArrayPtr()[i][j] == -1) {
        Game.state.getArrayPtr()[i][j] = 0;
      }
    }
  }
}

std::pair<int, int> getHeadCoordinates(Snake& Game) {
  std::pair<int, int> coordinates;
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (Game.state.getArrayPtr()[i][j] == 1) {
        coordinates.first = j;
        coordinates.second = i;
      }
    }
  }
  return coordinates;
}

void spawnAppleAt(Snake& Game, int x, int y) {
  Game.state.getArrayPtr()[y][x] = -1;
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
