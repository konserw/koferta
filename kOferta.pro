#-----------------------------------------------------------------------------
#
#    kOferta - system usprawniajacy proces ofertowania
#    Copyright (C) 2011  Kamil 'konserw' Strzempowicz, konserw@gmail.com
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#----------------------------------------------------------------------------

TARGET = kOferta

QT += core gui sql network

TEMPLATE = app

DEFINES += VER=1.9
DEFINES += GET_PASS=\\\"W7^Dz*mALE0UIOrEb\\\"

win32 { #nmake & M$ compiler
    QMAKE_CXXFLAGS += /nologo /O2 #/Wp64 /Wall
    QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMT

    CONFIG += qt windows release

    INCLUDEPATH += C:\\mysql\\include
    INCLUDEPATH += C:\\koferta_src

    LIBS += -L"C:\\Program Files\\Microsoft SDKs\\Windows\\v7.0A\\Lib" -lUser32 -lAdvAPI32
    LIBS += -LC:\\mysql\\lib -lmysqlclient -llibmysql

    DEFINES += WIN32
   # DEFINES += RELEASE
}

unix {
    QMAKE_CXXFLAGS += -Wall -Werror -Wextra
    LIBS += -L/usr/lib/mysql -lmysqlclient
}

OBJECTS_DIR =   temp/obj
UI_DIR =        temp/ui
MOC_DIR =       temp/moc
RCC_DIR =       temp/rcc

INCLUDEPATH += includes

SOURCES += \
    src/*.cpp \
    src/MainWindow/*.cpp \
    SzukajOferty.cpp \
    SzukajKlienta.cpp \
    SzukajTowaru.cpp \
    delegate.cpp \
    WyborTowaru.cpp

HEADERS  += includes/*.h \
    SzukajOferty.h \
    SzukajKlienta.h \
    SzukajTowaru.h \
    delegate.h \
    WyborTowaru.h

FORMS += ui/*.ui \
    SzukajOferty.ui \
    SzukajKlienta.ui \
    SzukajTowaru.ui \
    WyborTowaru.ui

RESOURCES += res/zasoby.qrc

OTHER_FILES += \
    changelog.txt \
    res/LICENSE \
    res/gpl-3.0.txt \
    res/gpl-3.0.html \
    res/SHA1_LICENSE \
    README.md
