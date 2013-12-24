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
#include "functions.h"
#include "Logger.h"
#include "MainWindow.h"
#include "Logowanie.h"
#include "User.h"
#include "Macros.h"

cUser* currentUser = NULL;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

#ifndef QT_NO_DEBUG_OUTPUT
if(Logger::instance()->setFilePath(filePath(".log")))
        qInstallMessageHandler(Logger::logHandler);
    else
        qDebug() << "Unable to create log file! Logging to std::cerr.";
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

    delete currentUser;
    return result;
}
