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

User::User() : uid(-1)
{
}

User::User(int Uid, QString Name, QString Phone, QString Mail, QString CharForOfferSymbol, bool Male, bool ResetPassword) :
    uid(Uid), name(Name), phone(Phone), mail(Mail), charForOfferSymbol(CharForOfferSymbol), male(Male), resetPassword(ResetPassword)
{
}

User::~User()
{
}

QString User::getName() const
{
    return name;
}

QString User::getPhone() const
{
    return phone;
}

QString User::getGenderSuffix() const
{
    if(male)
        return QString::null;
    else
        return QString("a");
}

QString User::getCharForOfferSymbol() const
{
    return charForOfferSymbol;
}

bool User::shouldChangePassword() const
{
    return resetPassword;
}

bool User::isValid() const
{
    return uid > -1;
}

QString User::getMail() const
{
    return mail;
}

bool User::getMale() const
{
    return male;
}

int User::getUid() const
{
    return uid;
}
