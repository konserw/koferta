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

#include <QtDebug>
#include "MerchandiseSelectionModel.hpp"

MerchandiseSelectionModel::MerchandiseSelectionModel(const QHash<int, double> &hash, QObject *parent) :
    MerchandiseSearchModel(parent),
    m_hash(hash)
{
}

int MerchandiseSelectionModel::columnCount(const QModelIndex &) const
{
    return 5;
}

Qt::ItemFlags MerchandiseSelectionModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    if(index.column() == 0)
        return Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled;
}

bool MerchandiseSelectionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role != Qt::EditRole || index.column() != 0)
        return false;

    int id = d(index, 0).toInt();
    double count = value.toDouble();

    if(m_hash.contains(id))
        m_hash[id] = count;
    else
        m_hash.insert(id, count);

    emit itemCountChanged(id, count);

    return true;
}

QVariant MerchandiseSelectionModel::data(const QModelIndex &index, int role) const
{
    if(!(role == Qt::DisplayRole || role == Qt::EditRole) || !index.isValid())
        return QVariant();

    if(role == Qt::EditRole && index.column() > 0)
        return QVariant();

    int section = index.column();

    int id = d(index, 0).toInt();

    switch(section)
    {
    case 0:
        if(m_hash.contains(id))
            return m_hash[id];
        return 0;
    case 1: return d(index, 1);
    case 2: return d(index, 2);
    case 3: return d(index, 3);
    case 4: return (d(index, 4).toString() == "m") ? tr("m.b.") : tr("szt.");
    }

    qWarning() << "View requested non-existant column, number:" << section;
    return QVariant();
}

QVariant MerchandiseSelectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();

    switch(section)
    {
    case 0: return tr("Ilość");
    case 1: return tr("Kod");
    case 2: return tr("Nazwa");
    case 3: return tr("Cena Kat. €");
    case 4: return tr("j/m");
    }

    qWarning() << "View requested non-existant column, number:" << section;
    return QVariant();
}

void MerchandiseSelectionModel::setHash(const QHash<int, double> &hash)
{
    m_hash = hash;
}
