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

#include <QString>

#ifndef USER_H
#define USER_H

/*!
 * \brief Klasa przechowująca dane użytkownika
 */
class User
{
    friend class Database;

public:
    User() = delete;
    User(const User&) = delete;
    void operator=(const User&) = delete;
    ~User();
    static User* current();
    static void dropUser();

    bool incrementOfferNumber();
    QString getName() const;
    QString getMail() const;
    QString getAddress() const;
    bool getMale() const;
    int getUid() const;
    int getCurrentOfferNumber() const;
    QString getCurrentOfferNumberWithYear() const;
    QString getPhone() const;
    QString getDbName() const;
    QString suffix() const;

protected:
    QString dbName;
    QString name;
    QString phone;
    QString mail;
    QString address;
    bool male;
    int uid;
    int currentOfferNumber;

    User(int Uid, const QString& Name, const QString& Phone, const QString& Mail, const QString& Address, bool Male, int CurrentOfferNumber, const QString& DbName);
    static User* instance;
};



#endif // USER_H
