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

QT += sql printsupport widgets

CONFIG += c++11

TEMPLATE = app

DEFINES += VER=2.4
DEFINES += ADMIN_PASS=\\\"\\\"
DEFINES += RELEASE
DEFINES += NOSSL

win32 {
    RC_FILE = res/koferta.rc

    QMAKE_CXXFLAGS += /nologo /O2

    QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMT
    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:LIBCMT

    CONFIG += qt windows #release

    DEFINES += WIN32    
    DEFINES += QT_NO_DEBUG_OUTPUT

    #LIBS += -L"C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x86" -lUser32 -lAdvAPI32
    #LIBS += -L"C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x64" -lUser32 -lAdvAPI32
    #INCLUDEPATH += "C:/Program Files/MySQL/MySQL Connector C 6.1 6.1.2/include"
    #LIBS += -L"C:/Program Files/MySQL/MySQL Connector C 6.1 6.1.2/lib" -llibmysql
    LIBS += -L"C:/Program Files (x86)/MySQL/MySQL Connector C 6.1/lib" -llibmysql
}

unix {
    QMAKE_CXXFLAGS += -Wall -Werror -Wextra
    LIBS += -L/usr/lib/mysql -lmysqlclient
}

DEPENDPATH += . res 

HEADERS += \
    Database.h \
    LoadDialog.h \
    Logowanie.h \
    Macros.h \
    MainWindow.h \
    User.h \
    EdycjaKombo.h  \
    functions.h \
    Logger.h \
    Merchandise.h \
    MerchandiseSearch.h \
    MerchandiseSearchModel.h \
    MerchandiseSelection.h \
    MerchandiseSelectionModel.h \
    MerchendiseSelectionDelegate.h \
    MerchandiseListDelegate.h \
    MerchandiseListModel.h \
    CustomerNew.h \
    CustomerEdit.h \
    MerchandiseNew.h \
    CustomerSearch.h \
    OfferSearch.h \
    CustomerSelection.h

FORMS += \
    LoadDialog.ui \
    Logowanie.ui \
    MainWindow.ui \
    EdycjaKombo.ui \
    MerchandiseSearch.ui \
    CustomerNew.ui \
    CustomerEdit.ui \
    MerchandiseNew.ui \
    OfferSearch.ui \
    CustomerSelection.ui

SOURCES += \
    Database.cpp \
    LoadDialog.cpp \
    Logowanie.cpp \
    Main.cpp \
    MainWindow.cpp \
    User.cpp \
    EdycjaKombo.cpp \
    Logger.cpp \
    Merchandise.cpp \
    MerchandiseSearch.cpp \
    MerchandiseSearchModel.cpp \
    MerchandiseSelection.cpp \
    MerchandiseSelectionModel.cpp \
    MerchendiseSelectionDelegate.cpp \
    MerchandiseListDelegate.cpp \
    MerchandiseListModel.cpp \
    CustomerNew.cpp \
    CustomerEdit.cpp \
    MerchandiseNew.cpp \
    CustomerSearch.cpp \
    OfferSearch.cpp \
    CustomerSelection.cpp

RESOURCES += \
    res/zasoby.qrc

TRANSLATIONS = \
    kOferta_pl.ts \
    kOferta_en.ts \
    kOferta_de.ts

OTHER_FILES += \
    LICENSE.md \
    README.md \
    hosts \
    res/koferta.rc
