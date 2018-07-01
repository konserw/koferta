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

#ifndef TERMITEM_H
#define TERMITEM_H

#include <QString>
#include <QHash>

enum class TermType : uint
{
    delivery = 0,
    offer,
    billing,
    deliveryDate,
    remarks
};
inline uint qHash(TermType key, uint seed)
{
    return qHash(static_cast<uint>(key), seed);
}


class TermItem
{
public:

    TermItem();
    TermItem(TermType Type, QString shortDesc, QString longDesc, int id = -1);

    int id() const;
    QString longDesc() const;
    QString shortDesc() const;
    TermType getType() const;

    void setLongDesc(const QString &longDesc);

private:
    TermType type;
    int m_id;
    QString m_shortDesc;
    QString m_longDesc;
};

#endif // TERMITEM_H
