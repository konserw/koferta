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
//#include <iostream>
#include <QFile>
#include <QDir>
#include <QApplication>

Logger* Logger::m_instance = nullptr;

void Logger::logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //Q_UNUSED(context);

    char tag;
    QString name;
    switch (type)
    {
    case QtDebugMsg:
        tag = 'D';
        name = "debug";
        break;
    case QtWarningMsg:
        tag = 'W';
        name = "Warning";
        break;
    case QtCriticalMsg:
        tag = 'C';
        name = "Error";
        break;
    case QtFatalMsg:
        tag = 'F';
        name = "FATAL ERROR";
        break;
    }

    QString debugInfo;
    if(type == QtDebugMsg)
        debugInfo = QString("[%1]\t").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
    else
        debugInfo = QString("[%1]\t%4 in %2, line %3:\n").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")).arg(context.function).arg(context.line).arg(name);

    (*m_out) << debugInfo << msg << endl;

    //emit logMsg(msg);

    if(type == QtFatalMsg)
    {
        abort();
    }
}

Logger::Logger()
{
#ifdef RELEASE
    QString path = QCoreApplication::applicationFilePath();
    QString suffix(".log");
#ifdef WIN32
    path = path.replace(".exe", suffix);
#else
    path = path + suffix;
#endif

    m_log = new QFile(path);
    if(m_log->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        m_out = new QTextStream(m_log);
        (*m_out) << "=========================================================================================\n";
    }
    else
    {
        m_out = new QTextStream(stdout);
        qDebug() << "Error opening log file '" << path << "'. All debug output redirected to stdout.";
    }
#else
    m_out = new QTextStream(stdout);
#endif
}

Logger::~Logger()
{
    delete m_out;
    delete m_log;
}

Logger *Logger::instance()
{
    if(m_instance == nullptr)
        m_instance = new Logger;

    return m_instance;
}

void Logger::logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Logger::instance()->logOutput(type, context, msg);
}
