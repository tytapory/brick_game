#include "snake.h"

Snake::State::State() {
  field = create_matrix(FIELD_H, FIELD_W);
  field[FIELD_H - 1][FIELD_W - 5] = 1;
  field[FIELD_H - 1][FIELD_W - 6] = 2;
  field[FIELD_H - 1][FIELD_W - 7] = 3;
  field[FIELD_H - 1][FIELD_W - 8] = 3;
  direction = Right;
  status = Start;
  score = 0;
  level = 1;
  pause = GameStart;
  speedMod = 1;
  emptyMatrix = create_matrix(FIELD_H, FIELD_W);
}

Snake::Snake() { spawnApple(); }

Snake::State::~State() {
  free_matrix(field, FIELD_H);
  free_matrix(emptyMatrix, FIELD_H);
}

int **Snake::State::getArrayPtr() { return field; }

void Snake::State::setStatus(int newStatus) {
  if (newStatus < GoUp || newStatus > GameOver) {
    throw std::out_of_range("New status is out of range");
  }
  status = newStatus;
}

int Snake::State::getStatus() { return status; }

void Snake::State::setDirection(int newDirection) {
  if (newDirection < Left || newDirection > Down) {
    throw std::out_of_range("New direction is out of range");
  }
  direction = newDirection;
}

int Snake::State::getDirection() { return direction; }

void Snake::turn(int newDirection) { state.setDirection(newDirection); }

void Snake::moveForward() {
  if (isInFrontWall()) {
    die();
  } else {
    if (isInFrontApple()) {
      spawnApple();
      state.setScore(state.getScore() + 1);
      if ((state.getScore() + 5) / 5 > state.getLevel()) state.addLevel();
    } else {
      killEnd();
    }
    addOneToSnake();
    placeHead();
    state.setStatus(Moving);
  }
}

bool Snake::isInFrontApple() { return getObjectInDirection() == -1; }

bool Snake::isInFrontWall() {
  int objInDir = getObjectInDirection();
  return objInDir == -2 || objInDir > 1;
}

int Snake::getObjectInDirection() {
  int result = 0;
  int i = 0;
  int j = 0;
  bool stop = false;
  for (; i < FIELD_H && !stop; i++) {
    for (; j < FIELD_W && !stop; j++) {
      if (state.getArrayPtr()[i][j] == 1) {
        stop = true;
      }
    }
    if (!stop)
      j = 0;
    else
      j--;
  }
  i--;
  switch (state.getDirection()) {
    case Up:
      i--;
      break;
    case Down:
      i++;
      break;
    case Left:
      j--;
      break;
    case Right:
      j++;
      break;
    default:
      break;
  }
  if (i < 0 || i >= FIELD_H || j < 0 || j >= FIELD_W)
    result = -2;
  else
    result = state.getArrayPtr()[i][j];
  return result;
}

void Snake::spawnApple() {
  std::vector<int *> listOfAllValidPlaces;
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (state.getArrayPtr()[i][j] == 0) {
        listOfAllValidPlaces.push_back(&(state.getArrayPtr()[i][j]));
      }
    }
  }
  if (listOfAllValidPlaces.size() > 0) {
    int random = rand() % listOfAllValidPlaces.size();
    *(listOfAllValidPlaces.at(random)) = -1;
  }
}

void Snake::killEnd() {
  int endNumber = getEndNumber();
  bool stop = false;
  for (int i = 0; i < FIELD_H && stop == false; i++) {
    for (int j = 0; j < FIELD_W && stop == false; j++) {
      if (state.getArrayPtr()[i][j] == endNumber) {
        stop = true;
        state.getArrayPtr()[i][j] = 0;
      }
    }
  }
}

void Snake::addOneToSnake() {
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (state.getArrayPtr()[i][j] > 0) {
        state.getArrayPtr()[i][j] = state.getArrayPtr()[i][j] + 1;
      }
    }
  }
}

void Snake::placeHead() {
  int i = 0;
  int j = 0;
  bool stop = false;
  for (; i < FIELD_H && !stop; i++) {
    for (; j < FIELD_W && !stop; j++) {
      if (state.getArrayPtr()[i][j] == 2) {
        stop = true;
      }
    }
    if (!stop)
      j = 0;
    else
      j--;
  }
  i--;
  switch (state.getDirection()) {
    case Up:
      i--;
      break;
    case Down:
      i++;
      break;
    case Left:
      j--;
      break;
    case Right:
      j++;
      break;
    default:
      break;
  }
  state.getArrayPtr()[i][j] = 1;
}

void Snake::die() {
  state.setStatus(GameOver);
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      state.getArrayPtr()[i][j] = 0;
    }
  }
  if (state.getScore() == 196) {
    int win[5][10] = {{1, 0, 0, 1, 0, 1, 0, 0, 0, 0},
                      {1, 0, 0, 1, 0, 0, 0, 1, 0, 0},
                      {1, 0, 1, 1, 0, 1, 0, 1, 1, 0},
                      {1, 0, 1, 1, 0, 1, 0, 1, 0, 1},
                      {0, 1, 1, 1, 0, 1, 0, 1, 0, 1}};
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 10; j++) {
        state.getArrayPtr()[7 + i][j] = win[i][j];
      }
    }
  } else {
    int lose[5][10] = {{1, 1, 0, 0, 1, 0, 0, 1, 0, 1},
                       {1, 0, 0, 1, 0, 1, 0, 0, 0, 1},
                       {1, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                       {1, 0, 0, 1, 1, 1, 0, 1, 0, 1},
                       {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}};
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 10; j++) {
        state.getArrayPtr()[7 + i][j] = lose[i][j];
      }
    }
  }
}

int Snake::getEndNumber() {
  int result = 1;
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (state.getArrayPtr()[i][j] > result) {
        result = state.getArrayPtr()[i][j];
      }
    }
  }
  return result;
}

GameInfo_t Snake::updateCurrentState() {
  if (!state.isPaused()) {
    moveForward();
  }
  GameInfo_t result;
  result.field = create_matrix(FIELD_H, FIELD_W);
  copy_matrix(result.field, state.getArrayPtr(), FIELD_H, FIELD_W);
  result.next = create_matrix(4, 4);
  copy_matrix(result.next, state.getEmptyMatrix(), 4, 4);
  result.score = state.getScore();
  result.high_score = state.getHighScore();
  result.level = state.getLevel();
  result.speed = (SNAKE_MINIMUM_SPEED - SNAKE_SPEED_STEP * result.level) /
                 state.getSpeedMod();
  result.pause = state.getPauseStatus();
  return result;
}

int Snake::State::getHighScore() {
  int high_score;
  FILE *file = fopen("./max_score_snek.txt", "r");
  fscanf(file, "%d", &high_score);
  fclose(file);
  setHighScore();
  return high_score;
}

void Snake::State::setScore(int newScore) { score = newScore; }

int Snake::State::getScore() { return score; }

void Snake::State::setHighScore() {
  FILE *file;
  int high_score;
  file = fopen("max_score_snek.txt", "r");
  fscanf(file, "%d", &high_score);
  fclose(file);
  if (score > high_score) {
    file = fopen("max_score_snek.txt", "w");
    fprintf(file, "%d", score);
    fclose(file);
  }
}

void Snake::State::addLevel() {
  if (level < 10) level++;
}

void Snake::State::clearLevel() { level = 0; }

int Snake::State::getLevel() { return level; }

void Snake::State::pauseGame() { pause = GamePause; }

void Snake::State::unpauseGame() { pause = None; }

bool Snake::State::isPaused() {
  return pause == GamePause || pause == GameStart;
}

int Snake::State::getPauseStatus() { return pause; }

void Snake::State::setSpeedMod(int mod) { speedMod = mod; }

int Snake::State::getSpeedMod() { return speedMod; }

void Snake::userInput(int action, bool hold) {
  if (state.getStatus() == Status_t::Start && action == UserAction_t::Start) {
    state.unpauseGame();
  }
  if (action == Pause && state.getStatus() != Status_t::Start) {
    if (state.isPaused()) {
      state.unpauseGame();
    } else {
      state.pauseGame();
    }
  }
  if (!state.isPaused() && action >= Left && action <= Down) {
    int oldDir = state.getDirection();
    turn(action);
    if (getObjectInDirection() == 2) {
      state.setDirection(oldDir);
      state.setSpeedMod(1);
    } else {
      if (hold == true)
        state.setSpeedMod(2);
      else
        state.setSpeedMod(1);
    }
  }
}

int **Snake::State::getEmptyMatrix() { return emptyMatrix; }