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
#include <exception>

#include "MainWindow.h"
#include "Logowanie.h"
#include "User.h"
#include "Macros.h"

#ifdef WIN32
    QTextStream* logFile = NULL;
#endif

cUser* currentUser = NULL;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifndef WIN32
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#else
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1250"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1250"));

    QFile file("log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(NULL, "Error", "Nie udało się otworzyć pliku z logiem");
        logFile = new QTextStream(stderr);
    }
    else
    {
        logFile = new QTextStream(&file);
        logFile->setCodec("UTF-8");
    }
    if(logFile == NULL)
    {
        QMessageBox::critical(NULL, "Error", "Nie udało się otworzyć pliku z logiem, ani podpiąć do strumienia błędów.\nNastąpi zakończenie programu.");
        return 2;
    }
#endif

    Logowanie* logw;
    logw = new Logowanie;

    int result;
    try
    {
        result = logw->exec();
    }
    catch (std::exception& e)
    {
        DEBUG << "[Logowanie] Standard exception: " << e.what();
    }
    catch(...)
    {
        DEBUG << "[Logowanie] Unknown exception";
    }

    DEBUG << "logw result: " << result;
    delete logw;

    if(result == QDialog::Accepted)
    {

        DEBUG << "Zalogowano jako uzytkownik " << currentUser->name();

        MainWindow w;
        w.showMaximized();

        DEBUG << "wchodzę do głównej pętli";

        try
        {
            result = a.exec();
        }
        catch (std::exception& e)
        {
            DEBUG << "[Mainwindow] Standard exception: " << e.what();
        }
        catch(...)
        {
            DEBUG << "[Mainwindow] Unknown exception";
        }
    }
    else
        DEBUG << "Zamknieto okno logowanie - wychodzę";

    DEBUG << "koniec programu, status: " << result;
#ifdef WIN32
    delete logFile;
#endif
    delete currentUser;
    return result;
}
