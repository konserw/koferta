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

#include "User.hpp"
#include "Database.hpp"

#include <QDate>
#include <QVariant>

User User::getUserFromDB(int id, const QString& password)
{
    User user;
    auto record = Database::getUserData(id, password);
    user.uid = record.value("id").toInt();
    user.name = record.value("name").toString();
    user.phone = record.value("phone").toString();
    user.mail = record.value("mail").toString();
    user.charForOfferSymbol = record.value("charForOfferSymbol").toString();
    user.male = record.value("male").toBool();
    user.resetPassword = record.value("resetPassword").toBool();
    return user;
}

User::User() : uid(-1)
{ }

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
    int newOfferNumber = Database::getNewOfferNumber(uid);
/* Example: I1804P01
    1 znak - oznaczenie biznesu,
2 i 3 znak - rok,
4 i 5 znak - miesiąc,
    6 znak - osoba ofertująca (czyli A – Agata, M – Marek, P – Patryk itd.),
7 i 8 znak - numeracja ofert danej osoby w danym miesiącu,
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

bool User::isValid() const
{
    return uid > -1;
}

QString User::getMail() const
{
    return mail;
}

bool User::isMale() const
{
    return male;
}

int User::getUid() const
{
    return uid;
}
