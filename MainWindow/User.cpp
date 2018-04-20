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

QString User::getNewOfferSymbol() const
{
    int newOfferNumber = Database::instance()->getNewOfferNumber();
/* Example: I1804P01
    1 znak - oznaczenie biznesu,
2 i 3 znak - rok,
4 i 5 znak – miesiąc,
    6 znak – osoba ofertująca (czyli A – Agata, M – Marek, P – Patryk itd.),
7 i 8 znak – numeracja ofert danej osoby w danym miesiącu,
*/
    return QString("I%1%2%3")
            .arg(QDate::currentDate().toString("yyMM"))
            .arg(charForOfferSymbol)
            .arg(QString::number(newOfferNumber).rightJustified(2, '0'));
}

bool User::shouldChangePassword() const
{
    return resetPassword;
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
