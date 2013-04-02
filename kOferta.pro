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

QT = core gui sql network

TEMPLATE = app

DEFINES += VER=2.2
DEFINES += GET_PASS=\\\"W7^Dz*mALE0UIOrEb\\\"

win32 { #nmake & M$ compiler
    QMAKE_CXXFLAGS += /nologo /O2 #/Wall

    QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMT
    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:LIBCMT

    CONFIG += qt windows release

    INCLUDEPATH += C:\\mysql\\include
    INCLUDEPATH += C:\\koferta_src

    LIBS += -L"C:\\Program Files\\Microsoft SDKs\\Windows\\v7.0A\\Lib" -lUser32 -lAdvAPI32
    LIBS += -LC:\\mysql\\lib -lmysqlclient -llibmysql

    DEFINES += WIN32
    DEFINES += RELEASE
}

unix {
    QMAKE_CXXFLAGS += -Wall -Werror -Wextra
    LIBS += -L/usr/lib/mysql -lmysqlclient
}

OBJECTS_DIR =   temp/obj
UI_DIR =        temp/ui
MOC_DIR =       temp/moc
RCC_DIR =       temp/rcc

DEPENDPATH += . res 

HEADERS += Database.h \
           EdycjaKlienta.h \
           EdycjaTowaru.h \
           LoadDialog.h \
           Logowanie.h \
           Macros.h \
           MainWindow.h \
           NowyKlient.h \
           NowyTowar.h \
           NowyUser.h \
           SHA1.h \
           SzukajKlienta.h \
           SzukajOferty.h \
           SzukajTowaru.h \
           User.h \
           WyborKlienta.h \
    WyborTowaru.h

FORMS += EdycjaKlienta.ui \
         EdycjaTowaru.ui \
         LoadDialog.ui \
         Logowanie.ui \
         MainWindow.ui \
         NowyKlient.ui \
         NowyTowar.ui \
         NowyUser.ui \
         SzukajKlienta.ui \
         SzukajOferty.ui \
         SzukajTowaru.ui \
         WyborKlienta.ui \ 
    WyborTowaru.ui
         
SOURCES += Database.cpp \
           EdycjaKlienta.cpp \
           EdycjaTowaru.cpp \
           LoadDialog.cpp \
           Logowanie.cpp \
           Main.cpp \
           MainWindow.cpp \
           NowyKlient.cpp \
           NowyTowar.cpp \
           NowyUser.cpp \
           SHA1.cpp \
           SzukajKlienta.cpp \
           SzukajOferty.cpp \
           SzukajTowaru.cpp \
           User.cpp \
           WyborKlienta.cpp \        
    WyborTowaru.cpp

RESOURCES += res/zasoby.qrc

OTHER_FILES += \
    changelog.txt \
    res/LICENSE \
    res/gpl-3.0.txt \
    res/gpl-3.0.html \
    res/SHA1_LICENSE \
    README.md
