/*! \file
    \brief Заголовочный файл с общими функциями, объявлениями и т.д. для
   нескольких игр.
*/

#ifndef BRICK_GAME_COMMON_H
#define BRICK_GAME_COMMON_H

#define FIELD_H 20
#define FIELD_W 10

#define ACTION_BUTTON 32
#define TERMINATE_BUTTON 27
#define PAUSE_BUTTON 112
#define START_BUTTON 115

#include <stdlib.h>

/*! \brief Структура которая содержит данные для отрисовки представлениями
    \param field указатель на двумерный массив основного поля игры.
    \param next указатель на двумерный массив поля NEXT (используется тетрисом)
    \param score текущий счет игрока
    \param high_score текущий рекорд игрока
    \param level текущий уровень сложности игрока
    \param speed текущая задержка между кадрами
    \param pause текущее состояние паузы (Banner из common.h)
*/
typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef enum { None, GameStart, GamePause } Banner;

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Освобождает память выделенную для матрицы
    \param[in] matrix указатель на матрицу
    \param[in] H количество строк в матрице
  */
void free_matrix(int **matrix, int H);

/*! \brief Создает новую двумерную динамическую матрицу
    \param[in] H количество строк в матрице
    \param[in] W количество столбцов в матрице
    \return указатель на созданную матрицу.
*/
int **create_matrix(int H, int W);

/*! \brief Создает копию матрицы
    \param[out] dest указатель куда будет записана новая матрица
    \param[in] src указатель откуда будет записана новая матрица
    \param[in] H количество строк в матрице
    \param[in] W количество столбцов в матрице
*/
void copy_matrix(int **dest, int **src, int H, int W);

#ifdef __cplusplus
}
#endif

#endif
