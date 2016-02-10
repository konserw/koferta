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

#include "Logger.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QCoreApplication::setOrganizationName("Konserw  Software");
    //QCoreApplication::setOrganizationDomain("koferta.no-ip.biz");
    QCoreApplication::setApplicationName("kOferta");

    if(Logger::instance()->isOpen())
        qInstallMessageHandler(Logger::logHandler);
    else
        qWarning() << "Unable to create log file! Logging to std::cerr.";

    MainWindow w;

    qDebug() << "launching application";

    return app.exec();
}
