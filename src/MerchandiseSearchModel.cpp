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
#include <QDate>
#include <QSqlTableModel>
#include "MerchandiseSearchModel.hpp"

const QString MerchandiseSearchModel::m_dateFmt = "dd.MM.yyyy";

MerchandiseSearchModel::MerchandiseSearchModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool MerchandiseSearchModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return d(left, 1).toString() < d(right, 1).toString();
}

bool MerchandiseSearchModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    return true;
    /****************** TO DO **********************
     * filtering by valid dates *****************/

    QString d1 = sourceModel()->data(sourceModel()->index(sourceRow, 5, sourceParent)).toString();
    QString d2 = sourceModel()->data(sourceModel()->index(sourceRow, 6, sourceParent)).toString();

    QDate from = QDate::fromString(d1, m_dateFmt);
    QDate to = QDate::fromString(d2, m_dateFmt);
    return (QDate::currentDate() > from) && (QDate::currentDate() < to);
}

int MerchandiseSearchModel::columnCount(const QModelIndex &/*index*/) const
{
    return 4;
}
/*
Qt::ItemFlags SzukajTowaruModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    if(index.column() == 0)
        return Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;

    return 0;
}
*/

QVariant MerchandiseSearchModel::data(const QModelIndex &index, int role) const
{
    if(!(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole) || !index.isValid())
        return QVariant();

    if(role == Qt::UserRole)
        return d(index, 0);

    if(role == Qt::EditRole && index.column() > 0)
        return QVariant();

    int section = index.column();

    switch(section)
    {
    case 0: return d(index, 1);
    case 1: return d(index, 2);
    case 2: return d(index, 3);
    case 3: return (d(index, 4).toString() == "m") ? tr("m.b.") : tr("szt.");
    }

    qWarning() << "View requested non-existant column, number:" << section;
    return QVariant();
}

QVariant MerchandiseSearchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();

    switch(section)
    {
    case 0: return tr("Kod");
    case 1: return tr("Nazwa");
    case 2: return tr("Cena Kat. €");
    case 3: return tr("j/m");
    }

    qWarning() << "View requested non-existant column, number:" << section;
    return QVariant();
}

void MerchandiseSearchModel::setFilter(const QString &filter)
{
    QSqlTableModel* t = qobject_cast<QSqlTableModel*>(sourceModel());
    if(t)
        t->setFilter(filter);
}

QVariant MerchandiseSearchModel::d(const QModelIndex &index, int col) const
{
    return sourceModel()->data(index.sibling(index.row(), col));
}
