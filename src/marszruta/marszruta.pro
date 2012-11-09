CONFIG      += designer plugin debug_and_release
TARGET      = $$qtLibraryTarget(marszrutaplugin)
TEMPLATE    = lib

HEADERS     = marszrutaPlugin.h
SOURCES     = marszrutaPlugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L. 

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

include(marszruta.pri)
