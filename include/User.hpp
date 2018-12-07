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

#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    static User getUserFromDB(int id, const QString &password);
    User();

    QString getName() const;
    QString getMail() const;
    bool isMale() const;
    int getUid() const;
    QString getPhone() const;
    bool shouldChangePassword() const;
    bool isValid() const;

    QString getGenderSuffix() const;
    QString getNewOfferSymbol() const;


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
