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

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include<QString>

class Customer
{
public:
    explicit Customer();
    Customer(
            const QString& ShortName,
            const QString& FullName,
            const QString& Title,
            const QString& Name,
            const QString& Surname,
            const QString& Address,
            int id = -1
            );

    QString concatedName() const;
    bool isValid() const;

    QString getShortName() const;
    void setShortName(const QString &value);

    QString getFullName() const;
    void setFullName(const QString &value);

    QString getTitle() const;
    void setTitle(const QString &value);

    QString getName() const;
    void setName(const QString &value);

    QString getSurname() const;
    void setSurname(const QString &value);

    QString getHtmlAddress() const;
    QString getAddress() const;
    void setAddress(const QString &value);

private:
    int id;
    QString shortName;
    QString fullName;
    QString title;
    QString name;
    QString surname;
    QString address;

    friend class Database;
};

#endif // CUSTOMER_H
