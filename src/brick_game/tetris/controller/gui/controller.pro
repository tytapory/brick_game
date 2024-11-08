TEMPLATE = app
TARGET = tetris_gui

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -fsanitize=address -g
QMAKE_LFLAGS += -fsanitize=address -g

CONFIG += link_clang_libs

SOURCES += ../../../../common/common.c ../../../../gui/desktop/window/window.cpp ../../../../gui/desktop/display/display.cpp controller.cpp ../../logic/tetris.c
HEADERS += ../../../../common/common.h ../../../../gui/desktop/view.h ../../../../gui/desktop/window/window.h ../../../../gui/desktop/display/display.h controller.h ../../logic/tetris.h