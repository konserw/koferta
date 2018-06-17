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

#ifndef SZUKAJTOWARUMODEL_H
#define SZUKAJTOWARUMODEL_H

#include <QSortFilterProxyModel>

class MerchandiseSearchModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MerchandiseSearchModel(QObject *parent = 0);

    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    virtual int columnCount(const QModelIndex &/*index*/) const;
   // virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual void setFilter(const QString & filter);

protected:
    static const QString m_dateFmt;

    QVariant d(const QModelIndex &index, int col) const;
};

#endif // SZUKAJTOWARUMODEL_H
