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
    User();
    User(int Uid, QString Name, QString Phone, QString Mail, QString CharForOfferSymbol, bool Male, bool ResetPassword);
    virtual ~User();

    QString getName() const;
    QString getMail() const;
    bool getMale() const;
    int getUid() const;
    QString getPhone() const;
    QString getCharForOfferSymbol() const;
    bool shouldChangePassword() const;
    bool isValid() const;

    QString getGenderSuffix() const;

protected:
    QString name;
    QString phone;
    QString mail;
    QString charForOfferSymbol;
    bool male;
    int uid;
    bool resetPassword;

};



#endif // USER_H
