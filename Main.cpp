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
#include <QtDebug>
#include <QApplication>
#include <QTextCodec>
#include <exception>
#include "functions.h"
#include "Logger.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

//#ifndef QT_NO_DEBUG_OUTPUT
    if(Logger::instance()->setFilePath(filePath(".log")))
        qInstallMessageHandler(Logger::logHandler);
    else
        qWarning() << "Unable to create log file! Logging to std::cerr.";
//#endif

    MainWindow w;
    w.showMaximized();

    qDebug() << "wchodzę do głównej pętli";

    try
    {
        return a.exec();
    }
    catch (std::exception& e)
    {
        qCritical() << "[Mainwindow] Standard exception: " << e.what();
    }
    catch(...)
    {
        qCritical() << "[Mainwindow] Unknown exception";
    }
}
