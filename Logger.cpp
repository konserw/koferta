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

#include "Logger.h"
#include <QDate>
#include <QDebug>
#include <QTextStream>
#include <iostream>
#include <QFile>
#include <QFile>

Logger* Logger::m_instance = NULL;

void Logger::logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    /*
    Q_UNUSED(context);

    char tag;
    switch (type)
    {
    case QtDebugMsg:
        tag = 'D';
        break;
    case QtWarningMsg:
        tag = 'W';
        break;
    case QtCriticalMsg:
        tag = 'C';
        break;
    case QtFatalMsg:
        tag = 'F';
        break;
    }
*/
    QString debugInfo;
    if(type == QtDebugMsg)
        debugInfo = QString("[%1]\t").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
    else
        debugInfo = QString("Error in %2, line %3:\n[%1]\t").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")).arg(context.function).arg(context.line);


    if(m_out)
        (*m_out) << debugInfo << msg << endl;
    else
        std::cerr << debugInfo.toStdString() << msg.toStdString() << std::endl;

    //emit logMsg(msg);

#ifdef CONSOLE_OUT
    std::cout << msg.toStdString() << std::endl;
#endif

    if(type == QtFatalMsg)
    {
        abort();
    }
}


Logger::Logger()
{
    m_out  = NULL;
    m_log = NULL;
    m_pathSet = false;
}

Logger::~Logger()
{
    delete m_out;
    delete m_log;
}

Logger *Logger::instance()
{
    if(m_instance == NULL)
        m_instance = new Logger;

    return m_instance;
}

void Logger::logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Logger::instance()->logOutput(type, context, msg);
}

bool Logger::setFilePath(const QString &path)
{
    if(m_pathSet)
    {
        delete m_out;
        delete m_log;
    }

    m_log = new QFile(path);
    if(!m_log->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "Error opening log file '" << path << "'. All debug output redirected to console.";
        return false;
    }

    m_out = new QTextStream(m_log);
    (*m_out) << "=========================================================================================\n";

    m_pathSet = true;
    return true;
}
