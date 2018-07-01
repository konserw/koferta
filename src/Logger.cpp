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

#include <QDate>
#include <QDebug>
#include <QTextStream>
#include <iostream>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

#include "Logger.hpp"

const char* Logger::messageTypes[] =
{
    "[   Debug   ]",
    "[  Warning  ]",
    "[   Error   ]",
    "[Fatal error]",
    "[    Info   ]"
};

void Logger::logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    (*m_out) << QStringLiteral("[%1]%2[%3:%4 (%5)] ")
                .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"))
                .arg(messageTypes[type])
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
             << msg << endl;

    if(type == QtFatalMsg)
    {
        abort();
    }
}

Logger::Logger()
{
    QDir dir(QDir::currentPath());
    dir.mkdir("log");
    dir.cd("log");
    QString logFilePath = dir.absoluteFilePath(QString("%1.log").arg(QDate::currentDate().toString("yyyy-MM-dd")));
    m_log = new QFile(logFilePath);
    if(m_log->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        m_out = new QTextStream(m_log);
        (*m_out) << "\nStarting kOferta version " << VERSION << endl;
    }
    else
    {
        m_out = new QTextStream(stdout);
        qDebug() << "Error opening log file '" << logFilePath << "'. All debug output redirected to stdout.";
    }
    //remove old logs - leave 7 most recent files
    const QStringList filter = {QString("*.log")};
    QStringList entryList = dir.entryList(filter, QDir::Files, QDir::Name | QDir::Reversed);
    for(int i=7; i < entryList.size(); ++i)
        dir.remove(entryList[i]);
}

Logger::~Logger()
{
    delete m_out;
    delete m_log;
}

Logger& Logger::instance()
{
    static Logger m_instance;
    return m_instance;
}

void Logger::logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
#ifdef LOG_TO_STDOUT
    std::cout << msg.toStdString() << std::endl;
#else
    Logger::instance().logOutput(type, context, msg);
#endif
}
