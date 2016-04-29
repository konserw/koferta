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

#include "Customer.h"

Customer::Customer()
{
    id = -1;
}

Customer::Customer(const QString &ShortName, const QString &FullName, const QString &Title, const QString &Name, const QString &Surname, const QString &Address, int id)
    : id(id), shortName(ShortName), fullName(FullName), title(Title), name(Name), surname(Surname), address(Address)
{

}

QString Customer::concatedName() const
{
    return QString("%1 %2 %3").arg(title).arg(name).arg(surname);
}

QString Customer::getAddress() const
{
    return address;
}

QString Customer::getShortName() const
{
    return shortName;
}

QString Customer::getFullName() const
{
    return fullName;
}
