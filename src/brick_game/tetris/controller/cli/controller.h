/*!\file
 * \brief Заголовочный файл контроллера для представления тетриса в терминале.
 */

#ifndef CLI_TETRIS_CONTROLLER_H
#define CLI_TETRIS_CONTROLLER_H

#include <unistd.h>

#include "../../../../gui/cli/console_front.h"
#include "../../logic/tetris.h"

/*!
    \brief получает ввод пользователя с клавиатуры и обрабатывает его
*/
void updateUserAction();

#endif