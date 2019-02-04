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

#include <QDebug>
#include "Logger.hpp"
#include "MainWindow.hpp"
#include "DatabaseHelpers.hpp"

#include "application.h"


int main(int argc, char *argv[]) {
    qInstallMessageHandler(Logger::logToFile);
    Application app(argc, argv);

    int ret;
    try {
        MainWindow w;
        ret = app.exec();
    } catch (const DatabaseException & e) {
        qCritical() << "[MAIN] DatabaseException has been thrown:" << e.what();
        return EXIT_FAILURE; // exit the application
    } catch (const std::exception & e) {
        qCritical() << "[MAIN] std::exception has been thrown:" << e.what();
        return EXIT_FAILURE; // exit the application
    } catch (...) {
        qCritical() << "[MAIN] Unknown Exception has been thrown";
        return EXIT_FAILURE; // exit the application
    }
    return ret;
}
