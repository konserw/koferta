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

#ifndef DATABASE_HELPERS_HPP
#define DATABASE_HELPERS_HPP

#include <stdexcept>
#include <QString>
#include <QtDebug>
#include <QCryptographicHash>
#include <QtSql>

class DatabaseException : public std::runtime_error
{
public:
    DatabaseException(QString text) : std::runtime_error(text.toStdString())
    {
        qCritical().noquote() << text;
    }
};

// Returns HMAC-SHA1 encrypted signature composed of public key and secret base string
// function pasted from https://developersarea.wordpress.com/2014/11/27/cryptographic-hashes-in-qt-hmac-sha1-pbkdf2-md5/
// copyright Neha Gupta
// with modifications by Kamil Strzempowicz
QByteArray hmacSha1(QByteArray key, QByteArray baseString);

// wraper for hmacSha1 for salting passwords
QString saltPassword(const QString& salt, const QString& password);

class Transaction
{
    static bool opened;
public:
    Transaction() = delete;
    static void open();
    static QSqlQuery run(const QString &queryText);
    static void run(QSqlQuery& query);
    static void runBatch(QSqlQuery& query);
    static void commit();
};

#endif // DATABASE_HELPERS_HPP
