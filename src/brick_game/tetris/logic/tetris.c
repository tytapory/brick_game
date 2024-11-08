#include "tetris.h"
GameInfo_t updateCurrentState() {
  State_t *state = get_current_state();
  return updateCurrentStateFrom(state);
}

GameInfo_t updateCurrentStateFrom(State_t* state) {
  GameInfo_t info;
  int **field = create_matrix(FIELD_H, FIELD_W);
  copy_matrix(field, state->field, FIELD_H, FIELD_W);

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->coord_x - i;
      int new_y = state->coord_y + j;
      if ((state->block[i][j] == 1) && (new_x >= 0) && (new_y < FIELD_W)) {
        field[new_x][new_y] = 1;
      }
    }
  }

  info.field = field;
  info.score = state->score;
  info.level = state->level;
  info.speed = state->speed;

  int **next = create_matrix(4, 4);
  copy_matrix(next, state->next_block, state->next_block_size,
              state->next_block_size);
  info.next = next;

  int high_score;
  FILE *file = fopen("./max_score_tetris.txt", "r");
  fscanf(file, "%d", &high_score);
  fclose(file);
  info.high_score = high_score;

  info.pause = state->pause;

  return info;
}

int **generate_new_block(int *block_size, Bag_T *bag) {
  int block_type = getFromBag(bag);

  if (block_type == ALPHA) {
    *block_size = 4;
  } else if (block_type == DELTA) {
    *block_size = 4;
  } else {
    *block_size = 4;
  }

  int **block = create_matrix(*block_size, *block_size);
  int **temp = create_matrix(*block_size, *block_size);

  if (block_type == ALPHA) {  // LINE
    block[0][0] = 1;
    block[0][1] = 1;
    block[0][2] = 1;
    block[0][3] = 1;
  } else if (block_type == BETA) {  // #
                                    // ##
                                    //  #  SNAKE
    block[1][0] = 1;
    block[0][0] = 1;
    block[0][1] = 1;
    block[0][2] = 1;
  } else if (block_type == GAMMA) {  // L REV
    block[1][2] = 1;
    block[0][0] = 1;
    block[0][1] = 1;
    block[0][2] = 1;
  } else if (block_type == DELTA) {  // CUBE
    block[0][0] = 1;
    block[0][1] = 1;
    block[1][0] = 1;
    block[1][1] = 1;
  } else if (block_type == OMEGA) {  // L
    block[0][0] = 1;
    block[0][1] = 1;
    block[1][1] = 1;
    block[1][2] = 1;
  } else if (block_type == PSI) {  // T
    block[0][0] = 1;
    block[0][1] = 1;
    block[0][2] = 1;
    block[1][1] = 1;
  } else {  // SNAKE REV
    block[1][0] = 1;
    block[1][1] = 1;
    block[0][1] = 1;
    block[0][2] = 1;
  }

  int rotations = rand() % 4;

  for (int i = 0; i < rotations; i++) {
    rotate(temp, block, *block_size);
    copy_matrix(block, temp, *block_size, *block_size);
  }

  free_matrix(temp, *block_size);
  return block;
}

char getFromBag(Bag_T *bag) {
  char amountOfBlocksInBag = bag->cube + bag->L + bag->L_reverse + bag->line +
                             bag->snake + bag->snake_rev + bag->T;
  char trueRandom = rand() % amountOfBlocksInBag;
  if (bag->line == 0) trueRandom++;
  if (bag->snake == 0 && trueRandom > 0) trueRandom++;
  if (bag->L_reverse == 0 && trueRandom > 1) trueRandom++;
  if (bag->cube == 0 && trueRandom > 2) trueRandom++;
  if (bag->L == 0 && trueRandom > 3) trueRandom++;
  if (bag->T == 0 && trueRandom > 4) trueRandom++;
  switch (trueRandom) {
    case 0:
      bag->line = 0;
      break;
    case 1:
      bag->snake = 0;
      break;
    case 2:
      bag->L_reverse = 0;
      break;
    case 3:
      bag->cube = 0;
      break;
    case 4:
      bag->L = 0;
      break;
    case 5:
      bag->T = 0;
      break;
    case 6:
      bag->snake_rev = 0;
      break;
    default:
      break;
  }
  if (amountOfBlocksInBag == 1) refill_bag(bag);
  return trueRandom;
}

void spawn_new_block() {
  spawn_new_block_from(get_current_state());
}

void spawn_new_block_from(State_t *state) {
  free_matrix(state->block, state->block_size);
  state->block_size = state->next_block_size;
  state->block = state->next_block;

  state->coord_x = -1;
  if (state->block_size == 2) {
    state->coord_y = 4;
  } else {
    state->coord_y = 3;
  }

  int next_block_size;
  int **next_block = generate_new_block(&next_block_size, &(state->bag));
  state->next_block_size = next_block_size;
  state->next_block = next_block;

  state->status = Moving;
  state->start_time = current_time_millis();
  state->time_left = state->speed;
}

void attach_block() {
  State_t *state = get_current_state();
  attach_block_from(state);
}

void attach_block_from(State_t *state) {
  int game_over = 0;

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->coord_x - i;
      int new_y = state->coord_y + j;

      if ((new_x >= 0) && (new_x < FIELD_H) && (new_y >= 0) &&
          (new_y < FIELD_W) && (state->block[i][j] == 1)) {
        state->field[new_x][new_y] = 1;
      } else if ((state->block[i][j] == 1) && (new_x < 0)) {
        game_over = 1;
      }
    }
  }

  if (game_over == 1) {
    state->status = GameOver;
  } else {
    consume_rows_from(state);
    state->status = Spawn;
  }
}

void consume_rows() {
  State_t *state = get_current_state();
  consume_rows_from(state);
}

void consume_rows_from(State_t *state) {
  int consumed_rows = 0;

  for (int i = FIELD_H - 1; i > 0; i--) {
    int count_blocks = 0;
    for (int j = 0; j < FIELD_W; j++) {
      count_blocks += state->field[i][j];
    }

    if (count_blocks == FIELD_W) {
      for (int z = i; z > 0; z--) {
        for (int j = 0; j < FIELD_W; j++) {
          state->field[z][j] = state->field[z - 1][j];
        }
      }
      i++;
      consumed_rows++;
    }
  }

  if (consumed_rows == 1) {
    state->score += 100;
  } else if (consumed_rows == 2) {
    state->score += 300;
  } else if (consumed_rows == 3) {
    state->score += 700;
  } else if (consumed_rows == 4) {
    state->score += 1500;
  }

  save_max_score_from(state);

  update_level_from(state);
}

void move_block_left() {
  State_t *state = get_current_state();
  move_block_left_from(state);
}

void move_block_left_from(State_t* state) {
  int can_move_left = 1;

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->coord_x - i;
      int new_y = state->coord_y - 1 + j;

      if ((state->block[i][j] == 1) && (new_y < 0)) {
        can_move_left = 0;
      } else if ((new_y < FIELD_W) && (new_x < FIELD_H) && (new_y >= 0) &&
                 (new_x >= 0)) {
        if ((state->block[i][j] + state->field[new_x][new_y]) == 2) {
          can_move_left = 0;
        }
      }
    }
  }

  if (can_move_left == 1) {
    (state->coord_y)--;
  }

  int attached = block_is_attached_from(state);

  if (attached == 0) {
    state->status = Moving;

  } else {
    state->status = Attaching;
  }
}

void move_block_right() {
  State_t *state = get_current_state();
  move_block_right_from(state);
}

void move_block_right_from(State_t *state) {
  int can_move_right = 1;

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->coord_x - i;
      int new_y = state->coord_y + 1 + j;

      if ((state->block[i][j] == 1) && (new_y >= FIELD_W)) {
        can_move_right = 0;
      } else if ((new_y < FIELD_W && new_y >= 0) &&
                 (new_x >= 0 && new_x < FIELD_H)) {
        if ((state->block[i][j] + state->field[new_x][new_y]) == 2) {
          can_move_right = 0;
        }
      }
    }
  }

  if (can_move_right == 1) {
    (state->coord_y)++;
  }

  int attached = block_is_attached_from(state);

  if (attached == 0) {
    state->status = Moving;

  } else {
    state->status = Attaching;
  }
}

int block_is_attached() {
  return block_is_attached_from(get_current_state());
}

int block_is_attached_from(State_t *state) {
  int attached = 0;

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->coord_x + 1 - i;
      int new_y = state->coord_y + j;

      if (state->block[i][j] == 1) {
        if (new_x >= FIELD_H) {
          attached = 1;
        } else if ((new_x >= 0) && (state->field[new_x][new_y] == 1)) {
          attached = 1;
        }
      }
    }
  }

  return attached;
}

int can_rotate_block(int **new_block) {
  return can_rotate_block_from(new_block, get_current_state());
}

int can_rotate_block_from(int **new_block, State_t *state) {
  int can_rotate = 1;

  int **old_block = state->block;

  state->block = new_block;

  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->coord_x - i;
      int new_y = state->coord_y + j;

      if (((new_y < 0) || (new_y >= FIELD_W)) && state->block[i][j] == 1) {
        can_rotate = 0;
      } else if ((new_y >= 0) && (new_y < FIELD_W) && (new_x >= 0) &&
                 (new_x < FIELD_H)) {
        if ((state->block[i][j] + state->field[new_x][new_y]) == 2) {
          can_rotate = 0;
        }
      }
    }
  }

  state->block = old_block;

  return can_rotate;
}

void rotate_block() {
  rotate_block_from(get_current_state());
}

void rotate_block_from(State_t *state) {
  int **new_block = create_matrix(state->block_size, state->block_size);
  rotate(new_block, state->block, state->block_size);

  if (can_rotate_block(new_block) == 1) {
    free_matrix(state->block, state->block_size);
    state->block = new_block;
  } else {
    free_matrix(new_block, state->block_size);
    ;
  }

  int attached = block_is_attached_from(state);

  if (attached == 0) {
    state->status = Moving;
  } else {
    state->status = Attaching;
  }
}

void refill_bag(Bag_T *bag) {
  bag->cube = 1;
  bag->L = 1;
  bag->L_reverse = 1;
  bag->line = 1;
  bag->snake = 1;
  bag->snake_rev = 1;
  bag->T = 1;
}

void initialize_state() {
  initialize_state_from(get_current_state());
}

void initialize_state_from(State_t *state) {
  state->status = Initial;

  int **field = create_matrix(FIELD_H, FIELD_W);

  state->field = field;
  state->score = 0;
  state->level = 1;
  state->speed = SPEED;
  state->time_left = state->speed;
  state->pause = GameStart;
  state->coord_x = -1;
  state->coord_y = 4;
  state->played_destroy_animation = 0;
  refill_bag(&(state->bag));

  int next_block_size;
  int **next_block = generate_new_block(&next_block_size, &(state->bag));
  int **block = create_matrix(next_block_size, next_block_size);
  copy_matrix(block, next_block, next_block_size, next_block_size);
  state->next_block_size = next_block_size;
  state->next_block = next_block;
  state->block_size = next_block_size;
  state->block = block;

  srand(current_time_millis());
}

State_t *get_current_state() {
  static State_t state_t;
  return &state_t;
}

void userInput(UserAction_t action) {
  State_t *state = get_current_state();
  userInputTo(action, state);
}

void userInputTo(UserAction_t action, State_t *state) {
  if ((action == Start) && (state->status == Initial)) {
    start_game_from(state);
  } else if ((action == Left) && (state->status == Moving)) {
    move_block_left_from(state);
  } else if ((action == Right) && (state->status == Moving)) {
    move_block_right_from(state);
  } else if ((action == Down) && (state->status == Moving)) {
    state->status = Attaching;
  } else if ((action == Action) && (state->status == Moving)) {
    rotate_block_from(state);
  } else if ((action == Pause) && (state->status != Initial)) {
    pause_game_from(state);
  } else if (action == Terminate) {
    free_matrix(state->field, FIELD_H);
    free_matrix(state->block, state->block_size);
    free_matrix(state->next_block, state->next_block_size);
  }
}

void rotate(int **new_, int **old, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      new_[i][j] = 0;
    }
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      new_[j][size - 1 - i] = old[i][j];
    }
  }
}

void finish_game() {
  State_t *state = get_current_state();
  finish_game_from(state);
}


void finish_game_from(State_t *state) {

  free_matrix(state->block, state->block_size);
  free_matrix(state->next_block, state->next_block_size);
  free_matrix(state->field, FIELD_H);

  initialize_state_from(state);
}

void shift_block() {
  shift_block_from(get_current_state());
}

void shift_block_from(State_t *state) {
  int attached = block_is_attached_from(state);
  if (attached == 0) {
    (state->coord_x)++;
    state->start_time = current_time_millis();
    state->time_left = state->speed;
    state->status = Moving;
  } else {
    state->status = Attaching;
  }
}

void save_max_score() {
  save_max_score_from(get_current_state());
}

void save_max_score_from(State_t *state) {
  FILE *file;

  int high_score;
  file = fopen("max_score_tetris.txt", "r");
  fscanf(file, "%d", &high_score);
  fclose(file);

  if (state->score > high_score) {
    file = fopen("max_score_tetris.txt", "w");
    fprintf(file, "%d", state->score);
    fclose(file);
  }
}

void update_level() {
  update_level_from(get_current_state());
} 

void update_level_from(State_t *state) {
  int new_level = state->score / NEW_LEVEL_TRESHOLD + 1;
  if (new_level > MAX_LEVEL) {
    new_level = MAX_LEVEL;
  }
  state->speed -= (new_level - state->level) * SPEED_STEP;
  state->level = new_level;
}

void start_game() {
  State_t *state = get_current_state();
  start_game_from(state);
}


void start_game_from(State_t *state) {
  state->pause = None;
  state->status = Spawn;
}

unsigned long long current_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (unsigned long long)(tv.tv_sec) * 1000 +
         (unsigned long long)(tv.tv_usec) / 1000;
}

unsigned long long process_timer() {
  return process_timer_from(get_current_state());
}

unsigned long long process_timer_from(State_t *state) {
  unsigned long long time_left;

  if (state->status == Initial) {
    time_left = -1;
  } else {
    unsigned long long elapsed_time = current_time_millis() - state->start_time;
    if (elapsed_time >= state->time_left) {
      time_left = 0;
    } else {
      state->time_left -= elapsed_time;
      state->start_time = current_time_millis();
      time_left = state->time_left;
    }
  }

  return time_left;
}

void pause_game() {
  pause_game_from(get_current_state());
}

void pause_game_from(State_t *state) {
  if (state->pause == None) {
    state->pause = GamePause;
    unsigned long long elapsed_time = current_time_millis() - state->start_time;
    if (elapsed_time >= state->time_left) {
      state->time_left = 0;
    } else {
      state->time_left -= elapsed_time;
    }

  } else {
    state->pause = None;
    state->start_time = current_time_millis();
  }
}
