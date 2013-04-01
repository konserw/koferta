#-------------------------------------------------
#
# Project created by konserw (C)
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = dbTools
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -Wall -Werror -Wextra
LIBS += -L/usr/lib/mysql -lmysqlclient

OBJECTS_DIR =   dbToolsTemp
UI_DIR =        dbToolsTemp
MOC_DIR =       dbToolsTemp
RCC_DIR =       dbToolsTemp

SOURCES += main.cpp \
    foo.cpp

HEADERS += \
    foo.h

RESOURCES += \
    zasoby.qrc
