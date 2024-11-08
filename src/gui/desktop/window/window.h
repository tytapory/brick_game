/*!\file
 * \brief Заголовочный файл в котором содержится стандартное окно для вывода
 * игры в оконном режиме.
 */
#ifndef WINDOW_H
#define WINDOW_H

#include <QPushButton>
#include <QWidget>

#include "../display/display.h"

class QPushButton;

/*! \brief Класс окна игры.
 *
 *  \param up_btn указатель на кнопку вверх.
 *  \param down_btn указатель на кнопку вниз.
 *  \param left_btn указатель на внопку влево.
 *  \param right_btn указатель на кнопку вправо.
 *  \param pause_btn указатель на кнопку пауза.
 *  \param big_btn указатель на кнопку действия.
 *  \param start_btn указатель на кнопку старт.
 *  \param shutdown_btn указатель на кнопку выхода.
 *  \param console_display указатель на дисплей консоли.
 * */
class ConsoleWindow : public QWidget {
 public:
  explicit ConsoleWindow(QWidget *parent = 0);
  ConsoleDisplay *console_display;
  QPushButton *up_btn;
  QPushButton *down_btn;
  QPushButton *left_btn;
  QPushButton *right_btn;
  QPushButton *pause_btn;
  QPushButton *big_btn;
  QPushButton *start_btn;
  QPushButton *shutdown_btn;
};

#endif  // WINDOW_H
