/*!
\file
\brief Заголовочный файл с описанием логики Тетриса

Файл в котором хранятся все необходимые компоненты для работы с логикой игры
"Тетриса"
*/

#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

#include "../../../common/common.h"

#define SPEED 500
#define SPEED_STEP 50
#define NEW_LEVEL_TRESHOLD 600
#define MAX_LEVEL 10

typedef enum { Initial, Spawn, Moving, Shifting, Attaching, GameOver } Status_t;

/*!
  \brief Структура которая содержит данные для работы механики "сумки"

  Структура представляет из себя сумку, в ней может либо быть элемент либо не
  быть. При опустошении сумки предполагается что она будет перезаполнена.
  Предполагается что если переменная равна 1 то блок если, если 0 то нет.
  \param line палка
  \param L L блок
  \param L обратный L блок
  \param cube квадрат
  \param snake змейка
  \param snake_rev обратная змейка
  \param T T блок
*/
typedef struct {
  char line;
  char L;
  char L_reverse;
  char cube;
  char snake;
  char snake_rev;
  char T;
} Bag_T;

/*!
  \brief Структура которая содержит данные о состоянии игры.

  \param status текущий статус игры
  \param field указатель на двумерную матрицу поля игры
  \param block указатель на двумерную матрицу текущего блока
  \param block_size размер квадратной матрицы текущего блока
  \param next_block указатель на двумерную матрицу следующего блока
  \param next_block_size размер квадратной матрицы следующего блока
  \param coord_x координата блока по X
  \param coord_y координата блока по Y
  \param score текущий счет
  \param level текущий уровень
  \param speed текущая задержка между кадрами
  \param pause текущий статус паузы (Banner из common.h)
  \param played_destroy_animation проиграна ли до конца анимация удаления блоков
  \param start_time время начала таймера
  \param time_left оставшееся время таймера
  \param bag сумка с блоками
*/
typedef struct {
  int status;
  int **field;
  int **block;
  int block_size;
  int **next_block;
  int next_block_size;
  int coord_x;
  int coord_y;
  int score;
  int level;
  int speed;
  int pause;
  char played_destroy_animation;
  unsigned long long start_time;
  unsigned long long time_left;
  Bag_T bag;
} State_t;

typedef enum { ALPHA, BETA, GAMMA, DELTA, OMEGA, PSI, ZETA } Block;
#ifdef __cplusplus
extern "C" {
#endif

/*!\brief Формирует информацию для представления

  \return информацию о состоянии игры для представления (GameInfo_t из common.h)
*/
GameInfo_t updateCurrentState();
GameInfo_t updateCurrentStateFrom(State_t* state);
/*!
  \brief Спавнит новый блок и ставит следующий по счету в матрицу NEXT
*/
void spawn_new_block_from(State_t *state);
void spawn_new_block();
/*!
  \brief Двигает блок влево
*/
void move_block_left();
void move_block_left_from(State_t *state);

/*!
  \brief Двигает блок вправо
*/
void move_block_right();
void move_block_right_from(State_t *state);
/*!
  \brief Двигает блок вниз
*/
void shift_block();
void shift_block_from(State_t *state);

/*!
  \brief Добавляет данные о текущем блоке к общей матрице и осуществляет логику
  удаления заполненных строк.
*/
void attach_block();
void attach_block_from(State_t *state);

/*!
  \brief Возвращает информацию о том, был ли прикреплен блок к общей матрице.

  \return 1 - если прикреплен, 0 - нет
*/
int block_is_attached();
int block_is_attached_from(State_t *state);

/*!
  \brief Содержит логику поворота блока.
*/
void rotate_block();
void rotate_block_from(State_t *state);
/*!
  \brief Инициализатор стартового состояния игры
*/
void initialize_state();
void initialize_state_from(State_t *state);

/*!
  \brief Осуществляет обработку ввода пользователя.
*/
void userInput(UserAction_t action);
void userInputTo(UserAction_t action, State_t* state);

/*!
  \brief Получает указатель на текущее состояние

  \return указатель на текущее состояние
*/
State_t *get_current_state();

/*!
  \brief Проверяет, возможно ли повернуть блок

  \param[in] new_block указатель на матрицу с блоком, который пытаемся повернуть

  \return 1 - можно, 0 - нет
*/
int can_rotate_block(int **new_block);
int can_rotate_block_from(int **new_block, State_t *state);
/*!
  \brief Фактически поворачивает блок.

  \param[out] new_ указатель на новую матрицу.
  \param[in] old указатель на старую матрицу блока
  \param[in] size размер матрицы блока
*/
void rotate(int **new_, int **old, int size);

/*!
  \brief Удаляет заполненные строки и начисляет очки
*/
void consume_rows();
void consume_rows_from(State_t *state);
/*!
  \brief Создает новый блок

  \param[in] block_size указатель на размер блоке
  \param[in] bag указатель на сумку

  \return указатель на новую матрицу с блоком
*/
int **generate_new_block(int *block_size, Bag_T *bag);

/*!
  \brief Завершает игру, освобождает ресурсы и перезапускает ее.
*/
void finish_game();
void finish_game_from(State_t *state);
/*!
  \brief Обработчик таймера игры

  \return оставшееся время до обновления игры
*/
unsigned long long process_timer();
unsigned long long process_timer_from(State_t *state);
/*!
  \brief Сохраняет рекорд игрока
*/
void save_max_score();
void save_max_score_from(State_t *state);

/*!
  \brief Обновляет уровень сложности
*/
void update_level();
void update_level_from(State_t *state);

/*!
  \brief Ставит игру в режим ее начала
*/
void start_game();
void start_game_from(State_t *state);

/*!
  \brief Получает текущее время в миллисекундах

  \return текущее время в миллисекундах
*/
unsigned long long current_time_millis();

/*!
  \brief Меняет статус паузы
*/
void pause_game();
void pause_game_from(State_t *state);
/*!
  \brief Получает блок из сумки

  \param[in] bag указатель на сумку

  \return индекс блока который был выбран
*/
char getFromBag(Bag_T *bag);

/*!
  \brief Перезаполняет сумку

  \param[in] bag указатель на сумку
*/
void refill_bag(Bag_T *bag);
#ifdef __cplusplus
}
#endif

#endif
