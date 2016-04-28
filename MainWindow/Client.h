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

#ifndef CLIENT_H
#define CLIENT_H

#include<QString>

class Client
{
public:
    Client(
        const QString& ShortName,
        const QString& FullName,
        const QString& Title,
        const QString& Name,
        const QString& Surname,
        const QString& Address
           );

    QString concatedName() const;
    QString getAddress() const;
    QString getShortName() const;
    QString getFullName() const;

private:
    QString shortName;
    QString fullName;
    QString title;
    QString name;
    QString surname;
    QString address;

    friend class Database;
};

#endif // CLIENT_H
