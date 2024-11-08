#ifndef S21_SNAKE_H_ADAPTER
#define S21_SNAKE_H_ADAPTER

#include <stdbool.h> 
#include "../../common/common.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Snake Snake;

Snake* Snake_new(); 
void Snake_delete(Snake* s);  
GameInfo_t Snake_updateCurrentState(Snake* s); 
void Snake_userInput(Snake* s, int action, bool hold); 
int getStatus(Snake* s);
#ifdef __cplusplus
}
#endif

#endif  // S21_SNAKE_H_ADAPTER

