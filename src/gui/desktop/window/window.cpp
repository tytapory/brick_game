#include "window.h"

ConsoleWindow::ConsoleWindow(QWidget *parent) : QWidget(parent) {
  setFixedSize(400, 750);

  up_btn = new QPushButton("/\\", this);
  up_btn->setGeometry(80, 40 + SIZE_Y, 40, 40);

  left_btn = new QPushButton("<", this);
  left_btn->setGeometry(40, 80 + SIZE_Y, 40, 40);

  right_btn = new QPushButton(">", this);
  right_btn->setGeometry(120, 80 + SIZE_Y, 40, 40);

  down_btn = new QPushButton("\\/", this);
  down_btn->setGeometry(80, 120 + SIZE_Y, 40, 40);

  big_btn = new QPushButton(" ", this);
  big_btn->setGeometry(250, 60 + SIZE_Y, 100, 100);

  pause_btn = new QPushButton("P", this);
  pause_btn->setGeometry(180, 40 + SIZE_Y, 40, 20);

  start_btn = new QPushButton("S", this);
  start_btn->setGeometry(180, 60 + SIZE_Y, 40, 20);

  shutdown_btn = new QPushButton("E", this);
  shutdown_btn->setGeometry(180, 120 + SIZE_Y, 40, 20);

  console_display = new ConsoleDisplay(this);
  console_display->setGeometry(20, 20, SIZE_X, SIZE_Y);
}