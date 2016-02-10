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

QT += sql printsupport widgets network

CONFIG += c++11

TEMPLATE = app

DEFINES += VER=2.6

win32 {
    RC_FILE = res/koferta.rc

    QMAKE_CXXFLAGS += /nologo /O2

    QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMT
    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:LIBCMT

    CONFIG += qt windows release

    DEFINES += RELEASE WIN32 QT_NO_DEBUG_OUTPUT
}

DEPENDPATH += . res 

HEADERS += \
    Database.h \
    LoadDialog.h \
    MainWindow.h \
    User.h \
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
    CustomerSelection.h \
    SettingsDialog.h \
    LoginDialog.h \
    AddConditionDialog.h \
    TermsChooserDialog.h \
    TermItem.h \
    TermModel.h \
    MerchandiseListView.h \
    LoadDialogMerchandiseListModel.h \
    LoadDialogMerchandiseListModelMySQL.h


FORMS += \
    LoadDialog.ui \
    MainWindow.ui \
    MerchandiseSearch.ui \
    CustomerNew.ui \
    CustomerEdit.ui \
    MerchandiseNew.ui \
    OfferSearch.ui \
    CustomerSelection.ui \
    SettingsDialog.ui \
    LoginDialog.ui \
    AddConditionDialog.ui \
    TermsChooserDialog.ui

SOURCES += \
    Database.cpp \
    LoadDialog.cpp \
    Main.cpp \
    MainWindow.cpp \
    User.cpp \
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
    CustomerSelection.cpp \
    SettingsDialog.cpp \
    LoginDialog.cpp \
    AddConditionDialog.cpp \
    TermsChooserDialog.cpp \
    TermItem.cpp \
    TermModel.cpp \
    MerchandiseListView.cpp \
    LoadDialogMerchandiseListModel.cpp \
    LoadDialogMerchandiseListModelMySQL.cpp

RESOURCES += \
    res/zasoby.qrc

TRANSLATIONS = \
    kOferta_pl.ts \
    kOferta_en.ts \
    kOferta_de.ts

OTHER_FILES += \
    LICENSE.md \
    README.md \
    res/koferta.rc
