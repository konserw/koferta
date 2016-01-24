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

#ifndef TERMMODEL_H
#define TERMMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "TermItem.h"

/*!
 * \brief Model that contain possible terms for offer, payment etc.
 */
class TermModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TermModel(QObject *parent = 0);
    virtual ~TermModel();

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column, const QModelIndex &) const;

    void insert(TermItem *item);

protected:
    QList<TermItem*> m_list;
};

#endif // TERMMODEL_H
