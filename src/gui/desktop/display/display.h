/*! \file
 *  \brief Заголовочный файл который содержит всю логику работы дисплея
 * виртуальной консоли.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#define SIZE_X 360
#define SIZE_Y 420

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QWidget>
#include <utility>
#include <vector>

#include "../../../common/common.h"

/*! \brief Класс дисплея окна, наследующийся от QWidget.
 *
 * Класс содержит все необходимые данные для отображения состояния игры,
 * эмуляции реальных дисплеев brick game, а также очень удобные функции для
 * работы и модификации дисплея. \param s21_animation_frame переменная которая
 * содержит текущий кадр анимации логотипа школы 21. \param image сожержит
 * текущее изображение на дисплее \param backgroundColor содержит информацию о
 * цвете фона дисплея. \param disabledPixelColor содержит информацию о цвете
 * "выключенного" пикселя. \param enabledPixelColor содержит информацию о цвете
 * "включенного" пикселя. \param frameData_ содержит информацию о текущем
 * состоянии игры (GameInfo_t из common.h)
 */
class ConsoleDisplay : public QWidget {
  Q_OBJECT  // Макрос Qt для поддержки сигналов и слотов

      public :
      /*! \brief Стандартный конструктор дисплея.
       */
      explicit ConsoleDisplay(QWidget* parent = nullptr);

  /*! \brief Функция отрисовывающая кадр исходя из переданного в нее состояния.

    \param[in] frameData Данные о состоянии игры (GameInfo_t из common.h)
   */
  void render(GameInfo_t frameData);

 private:
  int s21_animation_frame;
  QImage image;
  QColor enabledPixelColor;
  QColor disabledPixelColor;
  QColor backgroundColor;
  GameInfo_t frameData_;

  /*! \brief Событие вызывающееся при запросе на отрисовку нового кадра.
   *
   * Рисует на дисплее состояние игры исходя из frameData_
   * \param[in] event Область которую необходимо перерисовать
   */
  void paintEvent(QPaintEvent* event);

  /*!
   * \brief Отрисовывает пиксель на дисплее.
   *
   * Функция делит поле на сетку и исходя из настроек и переданных в нее
   * параметров рисует пиксель на поле.
   *
   * \param[in] painter отрисовщик изображения
   * \param[in] x положение на сетке по координате Х
   * \param[in] y положение на сетке по координате Y
   * \param[in] enable true - рисуем включенный пиксель, false - отключенный.
   */
  void drawPixel(QPainter& painter, int x, int y, bool enable = true);

  /*!
   * \brief Рисует отключенный пиксель.
   *
   * Небольшая абстракция функции drawPixel рисующая отключенный пискель.
   *
   * \param[in] painter отрисовщик изображения
   * \param[in] x положение на сетке по координате X
   * \param[in] y положение на сетке по координате Y
   */
  void disablePixel(QPainter& painter, int x, int y);

  /*!
   * \brief Рисует включенный пиксель.
   *
   * Небольшая абстракция функции drawPixel рисующая включенный пискель.
   * \param[in] painter отрисовщик изображения
   * \param[in] x положение на сетке по координате X
   * \param[in] y положение на сетке по координате Y
   */
  void enablePixel(QPainter& painter, int x, int y);

  /*! \brief Перезаписывает frameData_ исходя из нового состояния игры.
   *
   * \param[in] frameData новое состояние игры (GameInfo_t из common.h)
   */
  void updateGameInfo(GameInfo_t frameData);

  /*! \brief Рисует значок паузы исходя из ее состояния.
   *  \param[in] painter отрисовщик изображения.
   */
  void drawPauseStatus(QPainter& painter);

  /*! \brief Рисует логотип Школы 21
   *
   * Рисует кадр анимации логотипа школы.
   *
   * \param[in] painter отрисовщик изображения.
   */
  void drawLogo(QPainter& painter);

  /*! \brief Рисует текущий счет игрока
   *
   *  \param[in] painter отрисовщик изображения.
   */
  void drawScore(QPainter& painter);

  /*! \brief Рисует рекорд игрока
   *
   * \param[in] painter отрисовщик изображения.
   *
   */
  void drawHiScore(QPainter& painter);

  /*! \brief Рисует текущий уровень сложности.
   *
   * \param[in] painter отрисовщик изображения.*/
  void drawLvl(QPainter& painter);

  /*! \brief Очищает строку чисел

    Заполняет выбранную строку выключенными восьмерками.

      \param[in] painter отрисовщик изображения
      \param[in] y индекс строки
      \param[in] numOfDigits длина строки
   */
  void emptyScore(QPainter& painter, int y, int numOfDigits);

  /*!
   *  \brief Рисует одну цифру по заданным координатам.
   *
   *  Представляет положение цифр как сетку, и ставит цифру на заданных
   * координатах сетки. \param[in] painter отрисовщик изображения. \param[in]
   * posX положение цифры по координате X. \param[in] posY положение цифры по
   * координате Y. \param[in] number цифра для отрисовки.
   *
   * */
  void drawNumber(QPainter& painter, int posX, int posY, int number,
                  bool enabled = true);

  /*!
   * \brief Рисует вертикальный прямоугольник по координатам.
   *
   * \param[in] painter отрисовщи изображения.
   * \param[in] coordinates координаты в pair<int, int> где first это x, а
   * second - y. \param[in] size размеры сторон прямоугольника в pair<int, int>
   * где большая сторона - first, а меньшая - second. \param[in] enabled true -
   * цвет включенного пикселя, false - выключенного.
   */
  void drawVerticalLine(QPainter& painter, std::pair<int, int> coordinates,
                        std::pair<int, int> size, bool enabled = true);

  /*! \brief Рисует горизонтальный прямоугольник по координатам
   *
   * \param[in] painter отрисовщи изображения.
   * \param[in] coordinates координаты в pair<int, int> где first это x, а
   * second - y. \param[in] size размеры сторон прямоугольника в pair<int, int>
   * где большая сторона - first, а меньшая - second. \param[in] enabled true -
   * цвет включенного пикселя, false - выключенного.
   */
  void drawHorizontalLine(QPainter& painter, std::pair<int, int> coordinates,
                          std::pair<int, int> size, bool enabled = true);

  /*!\brief Рисует прямоугольник по координатам.
   *
   * \param[in] painter отрисовщи изображения.
   * \param[in] coordinates координаты в pair<int, int> где first это x, а
   * second - y. \param[in] size размеры сторон прямоугольника в pair<int, int>
   * где большая сторона - first, а меньшая - second. \param[in] enabled true -
   * цвет включенного пикселя, false - выключенного. \param[in] vertical true -
   * прямоугольник будет вертикальным, false - горизонтальным.
   */
  void drawLine(QPainter& painter, std::pair<int, int> coordinates,
                std::pair<int, int> size, bool enabled = true,
                bool vertical = true);
};

#endif  // DISPLAY_H
