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

User::User()
{
    uid = -1;
}

User::~User()
{
}

User& User::current()
{
    static User instance;
    return instance;
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

QString User::suffix() const
{
    if(male)
        return QString::null;
    else
        return QString("a");
}

bool User::shouldChangePassword() const
{
    return resetPassword;
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
