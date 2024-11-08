/*!\file
 * \brief Заголовочный файл контроллера для представления змейки в терминале.
 */

#ifndef CLI_RACE_CONTROLLER_H
#define CLI_RACE_CONTROLLER_H

#include <unistd.h>

#include "../../../../gui/cli/console_front.h"


void initialize();
void renderScr(GameInfo_t info);
int getInput();

#endif

