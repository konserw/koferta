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

#ifndef TOWARPROXY_H
#define TOWARPROXY_H

#include "MerchandiseSearchModel.hpp"
#include <QHash>

class MerchandiseSelectionModel : public MerchandiseSearchModel
{
    Q_OBJECT

public:  
    MerchandiseSelectionModel(const QHash<int, double>& hash, QObject *parent = nullptr);

    virtual int columnCount(const QModelIndex &) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual void setHash(const QHash<int, double>& hash);

signals:
    void itemCountChanged(int id, double count);

protected:
    QHash<int, double> m_hash;

};

#endif // TOWARPROXY_H
