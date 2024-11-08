#include "display.h"

ConsoleDisplay::ConsoleDisplay(QWidget *parent) : QWidget(parent) {
  backgroundColor = QColor(qRgb(138, 154, 118));
  disabledPixelColor = QColor(qRgb(132, 146, 113));
  enabledPixelColor = QColor(qRgb(1, 1, 1));
  image = QImage(SIZE_X, SIZE_Y, QImage::Format_RGB32);
  image.fill(Qt::black);
  frameData_.score = 0;
  frameData_.high_score = 0;
  frameData_.level = 0;
  frameData_.speed = 0;
  frameData_.pause = 0;
  frameData_.field = nullptr;
  frameData_.next = nullptr;
  s21_animation_frame = 0;
}

void ConsoleDisplay::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.fillRect(event->rect(), QBrush(backgroundColor));

  QColor pixelColor(enabledPixelColor);
  painter.setPen(QPen(pixelColor, 2));
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(5, 5, SIZE_X - 10, SIZE_Y - 10);

  drawScore(painter);
  drawHiScore(painter);
  drawLvl(painter);
  drawLogo(painter);
  drawPauseStatus(painter);

  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      disablePixel(painter, j, i);
    }
  }

  if (frameData_.field != nullptr) {
    for (int i = 0; i < FIELD_H; i++) {
      for (int j = 0; j < FIELD_W; j++) {
        if (frameData_.field[i][j] != 0) {
          enablePixel(painter, j, i);
        }
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      disablePixel(painter, j + 12, i + 15);
    }
  }

  if (frameData_.next != nullptr) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (frameData_.next[i][j] != 0) {
          enablePixel(painter, j + 12, i + 15);
        }
      }
    }
  }
}

void ConsoleDisplay::drawPixel(QPainter &painter, int x, int y, bool enable) {
  int distanceFromBorder = 10;
  int distanceBetweenPixels = 2;
  int pixelSide = 18;
  int outerWidth = 2;
  int distanceBetweenOuterAndInner = 2;

  int adjustedPixelSide = pixelSide - outerWidth;
  int innerSide = pixelSide - 2 * (distanceBetweenOuterAndInner + outerWidth);
  int realPixelX = distanceFromBorder + (pixelSide + distanceBetweenPixels) * x;
  int realPixelY = distanceFromBorder + (pixelSide + distanceBetweenPixels) * y;
  int realOuterX = realPixelX + outerWidth / 2;
  int realOuterY = realPixelY + outerWidth / 2;
  int realInnerX = realPixelX + distanceBetweenOuterAndInner + outerWidth;
  int realInnerY = realPixelY + distanceBetweenOuterAndInner + outerWidth;

  QColor pixelColor(enable == true ? enabledPixelColor : disabledPixelColor);
  painter.setPen(QPen(pixelColor, outerWidth));
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(realOuterX, realOuterY, adjustedPixelSide,
                   adjustedPixelSide);

  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(pixelColor));
  painter.fillRect(realInnerX, realInnerY, innerSide, innerSide, pixelColor);
}

void ConsoleDisplay::disablePixel(QPainter &painter, int x, int y) {
  drawPixel(painter, x, y, false);
}

void ConsoleDisplay::enablePixel(QPainter &painter, int x, int y) {
  drawPixel(painter, x, y, true);
}

void ConsoleDisplay::updateGameInfo(GameInfo_t frameData) {
  free_matrix(frameData_.field, FIELD_H);
  free_matrix(frameData_.next, 4);
  frameData_ = frameData;
}

void ConsoleDisplay::render(GameInfo_t frameData) {
  updateGameInfo(frameData);
  update();
}

void ConsoleDisplay::drawScore(QPainter &painter) {
  emptyScore(painter, 0, 5);
  int score = frameData_.score;
  int digits = 0;
  do {
    digits++;
    score /= 10;
  } while (score > 0);
  score = frameData_.score;
  for (int i = 0; i < digits; i++) {
    drawNumber(painter, i, 0, score % 10, true);
    score /= 10;
  }
}

void ConsoleDisplay::drawHiScore(QPainter &painter) {
  emptyScore(painter, 2, 5);
  int score = frameData_.high_score;
  int digits = 0;
  do {
    digits++;
    score /= 10;
  } while (score > 0);
  score = frameData_.high_score;
  for (int i = 0; i < digits; i++) {
    drawNumber(painter, i, 2, score % 10, true);
    score /= 10;
  }
}

void ConsoleDisplay::drawLvl(QPainter &painter) {
  emptyScore(painter, 4, 2);
  int score = frameData_.level;
  int digits = 0;
  do {
    digits++;
    score /= 10;
  } while (score > 0);
  score = frameData_.level;
  for (int i = 0; i < digits; i++) {
    drawNumber(painter, i, 4, score % 10, true);
    score /= 10;
  }
}

void ConsoleDisplay::emptyScore(QPainter &painter, int y, int numOfDigits) {
  for (int i = 0; i < numOfDigits; i++) drawNumber(painter, i, y, 8, false);
}

void ConsoleDisplay::drawNumber(QPainter &painter, int posX, int posY,
                                int number, bool enabled) {
  int widthOfRow = 3;
  int lenOfRow = 6;
  int distanceBetweenNums = 2;
  int distanceFromBorder = 10;
  int numberX = SIZE_X - distanceFromBorder -
                (posX + 1) * (lenOfRow + 2 * widthOfRow + distanceBetweenNums);
  int numberY = distanceFromBorder + (posY + 1) * 3 * widthOfRow +
                (posY + 1) * 2 * lenOfRow + posY * distanceBetweenNums;
  std::pair<int, int> size(widthOfRow, lenOfRow);

  // we will use classic approach to draw nums, like in calculators or in the
  // real brick game, we have 7 lines that will form number. here i will get
  // every line coordinate and then based on giver num will draw it using
  // drawLine func

  // 0--
  // 1--    horisontal
  // 2--

  // 0|    2|    vertical
  // 1|    3|

  //   --
  // |    |
  //   --      result
  // |    |
  //   --

  std::vector<std::pair<int, int>> verticalRows;
  std::vector<std::pair<int, int>> horizontalRows;
  horizontalRows.push_back(std::pair<int, int>(numberX + widthOfRow, numberY));
  horizontalRows.push_back(std::pair<int, int>(
      numberX + widthOfRow, numberY + widthOfRow + lenOfRow));
  horizontalRows.push_back(std::pair<int, int>(
      numberX + widthOfRow, numberY + 2 * widthOfRow + 2 * lenOfRow));

  verticalRows.push_back(std::pair<int, int>(numberX, numberY + widthOfRow));
  verticalRows.push_back(
      std::pair<int, int>(numberX, numberY + widthOfRow * 2 + lenOfRow));
  verticalRows.push_back(std::pair<int, int>(numberX + widthOfRow + lenOfRow,
                                             numberY + widthOfRow));
  verticalRows.push_back(std::pair<int, int>(
      numberX + widthOfRow + lenOfRow, numberY + widthOfRow * 2 + lenOfRow));

  switch (number) {
    case 0:
      drawHorizontalLine(painter, horizontalRows[0], size, enabled);
      drawHorizontalLine(painter, horizontalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[0], size, enabled);
      drawVerticalLine(painter, verticalRows[1], size, enabled);
      drawVerticalLine(painter, verticalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
    case 1:
      drawVerticalLine(painter, verticalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
    case 2:
      drawHorizontalLine(painter, horizontalRows[0], size, enabled);
      drawHorizontalLine(painter, horizontalRows[1], size, enabled);
      drawHorizontalLine(painter, horizontalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[1], size, enabled);
      break;
    case 3:
      drawHorizontalLine(painter, horizontalRows[0], size, enabled);
      drawHorizontalLine(painter, horizontalRows[1], size, enabled);
      drawHorizontalLine(painter, horizontalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
    case 4:
      drawHorizontalLine(painter, horizontalRows[1], size, enabled);
      drawVerticalLine(painter, verticalRows[0], size, enabled);
      drawVerticalLine(painter, verticalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
    case 5:
      drawHorizontalLine(painter, horizontalRows[0], size, enabled);
      drawHorizontalLine(painter, horizontalRows[1], size, enabled);
      drawHorizontalLine(painter, horizontalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[0], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
    case 6:
      drawHorizontalLine(painter, horizontalRows[0], size, enabled);
      drawHorizontalLine(painter, horizontalRows[1], size, enabled);
      drawHorizontalLine(painter, horizontalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[0], size, enabled);
      drawVerticalLine(painter, verticalRows[1], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
    case 7:
      drawHorizontalLine(painter, horizontalRows[0], size, enabled);
      drawVerticalLine(painter, verticalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
    case 8:
      drawHorizontalLine(painter, horizontalRows[0], size, enabled);
      drawHorizontalLine(painter, horizontalRows[1], size, enabled);
      drawHorizontalLine(painter, horizontalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[0], size, enabled);
      drawVerticalLine(painter, verticalRows[1], size, enabled);
      drawVerticalLine(painter, verticalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
    case 9:
      drawHorizontalLine(painter, horizontalRows[0], size, enabled);
      drawHorizontalLine(painter, horizontalRows[1], size, enabled);
      drawHorizontalLine(painter, horizontalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[0], size, enabled);
      drawVerticalLine(painter, verticalRows[2], size, enabled);
      drawVerticalLine(painter, verticalRows[3], size, enabled);
      break;
  }
}

void ConsoleDisplay::drawVerticalLine(QPainter &painter,
                                      std::pair<int, int> coordinates,
                                      std::pair<int, int> size, bool enabled) {
  drawLine(painter, coordinates, size, enabled, true);
}

void ConsoleDisplay::drawHorizontalLine(QPainter &painter,
                                        std::pair<int, int> coordinates,
                                        std::pair<int, int> size,
                                        bool enabled) {
  drawLine(painter, coordinates, size, enabled, false);
}

void ConsoleDisplay::drawLine(QPainter &painter,
                              std::pair<int, int> coordinates,
                              std::pair<int, int> size, bool enabled,
                              bool vertical) {
  QColor pixelColor(enabled == true ? enabledPixelColor : disabledPixelColor);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(pixelColor));
  painter.fillRect(coordinates.first, coordinates.second,
                   vertical == true ? size.first : size.second,
                   vertical == false ? size.first : size.second, pixelColor);
}

void ConsoleDisplay::drawLogo(QPainter &painter) {
  painter.setPen(
      QPen((s21_animation_frame % 2 == 1 || s21_animation_frame > 12) &&
                   s21_animation_frame > 4
               ? disabledPixelColor
               : enabledPixelColor,
           2));
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(250, 200, 30, 10);
  painter.drawRect(260, 220, 20, 10);
  painter.drawRect(260, 240, 30, 10);
  painter.drawRect(280, 210, 10, 10);
  painter.drawRect(250, 230, 10, 10);

  painter.setPen(
      QPen((s21_animation_frame % 2 == 0 || s21_animation_frame > 12) &&
                   s21_animation_frame > 4
               ? disabledPixelColor
               : enabledPixelColor,
           2));
  painter.drawRect(295, 200, 10, 10);
  painter.drawRect(305, 210, 10, 40);
  if (s21_animation_frame < 16)
    s21_animation_frame++;
  else
    s21_animation_frame = 0;
}

void ConsoleDisplay::drawPauseStatus(QPainter &painter) {
  QColor pixelColor(frameData_.pause != None ? enabledPixelColor
                                             : disabledPixelColor);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(pixelColor));
  painter.drawRect(325, 160, 5, 15);
  painter.drawRect(332, 160, 5, 15);
}