/**
    kOferta - system usprawniajacy proces ofertowania
    Copyright (C) 2011  Kamil 'konserw' Strzempowicz, konserw@gmail.com

    this program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    this program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "User.h"
#include "Database.h"

#include <QDate>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QtDebug>

User* User::instance = nullptr;

User::User(int Uid, const QString &Name, const QString &Phone, const QString &Mail, const QString &Address, bool Male, int CurrentOfferNumber, const QString& DbName)
    : uid(Uid), name(Name), phone(Phone), mail(Mail), address(Address), male(Male), currentOfferNumber(CurrentOfferNumber), dbName(DbName)
{
    qDebug() << "Creating new user:" << uid << Name << Phone << Mail << Address << Male << CurrentOfferNumber;
}

User::~User()
{
}

User *User::current()
{
    if(!instance)
        instance = Database::instance()->userInfo();
    return instance;
}

void User::dropUser()
{
    delete instance;
    instance = nullptr;
}

bool User::incrementOfferNumber()
{
    if(Database::instance()->setCurrentOfferNumber(currentOfferNumber+1))
    {
        currentOfferNumber++;
        return true;
    }
    return false;
}

QString User::getName() const
{
    return name;
}

QString User::getPhone() const
{
    return phone;
}

QString User::getDbName() const
{
    return dbName;
}

QString User::suffix() const
{
    if(male)
        return QString::null;
    else
        return QString("a");
}

QString User::getMail() const
{
    return mail;
}

QString User::getAddress() const
{
    return address;
}

bool User::getMale() const
{
    return male;
}

int User::getUid() const
{
    return uid;
}

int User::getCurrentOfferNumber() const
{
    return currentOfferNumber;
}

QString User::getCurrentOfferNumberWithYear() const
{
    return QString("%1/%2").arg(currentOfferNumber).arg(QDate::currentDate().toString("yyyy"));
}
