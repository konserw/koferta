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

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class QIODevice;
class QTextStream;
class QFile;

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger* instance();
    static void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

signals:
    void logMsg(QString);

protected:
    Logger();
    virtual ~Logger();

    static Logger* m_instance;
    QTextStream *m_out;
    QFile* m_log;
};

#endif // LOGGER_H
