/**
    kOferta - system usprawniajacy proces ofertowania
    Copyright (C) 2011  Kamil 'konserw' Strzempowicz, konserw@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <QApplication>
#include <QTextCodec>

#include "MainWindow.h"
#include "Logowanie.h"
#include "User.h"
#include "Macros.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    cUser** u;
    u = new cUser*;
    cLogowanie* logw;
    logw = new cLogowanie(u);

    if(logw->exec() != QDialog::Accepted)
    {
        delete logw;
        DEBUG << "Zamknieto okno logowanie - wychodzę";
        return 1;
    }

    delete logw;

    DEBUG << "Zalogowano jako uzytkownik " << (*u)->name();

    MainWindow w(*u);
    w.showMaximized();

    delete u;

    DEBUG << "wchodzę do głównej pętli";
    return a.exec();
}
