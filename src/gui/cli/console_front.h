/*! \file
 *  \brief Заголовочный файл в котором содержится все необходимое для работы с
 * консольным представлением.
 */

#ifndef CONSOLE_FRONT_H
#define CONSOLE_FRONT_H

#include <ncurses.h>

#include "../../common/common.h"

/*!
    \brief Отрисовывает игру в терминале

    \param info информация о статусе игры (GameInfo_t из common.h)
*/
void render(GameInfo_t info);

#endif
