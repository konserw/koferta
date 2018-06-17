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

#ifndef TOWARMODEL_H
#define TOWARMODEL_H

#include "Offer.hpp"
#include <QAbstractTableModel>
#include <QHash>
#include <QList>

class Merchandise;
class QSqlTableModel;
class QSqlRecord;

/*!
 * \brief Model przechowywujący listę oferowanych towarów
 */
class MerchandiseListModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class Database;
public:
    explicit MerchandiseListModel(QObject *parent = 0);
    virtual ~MerchandiseListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild);

    virtual void sort(int column, Qt::SortOrder order);

    void addItem(Merchandise* towar);

    bool isPLN() const;
    void setPLN(bool value);
    double getExchangeRate() const;
    void setGlobalRabat(double r);

    bool isRabat(const QModelIndex & i) const;
    bool isIlosc(const QModelIndex & i) const;

    QHash<int, double> hash() const;
    QString print(const int w, Offer::PrintOptions printOptions) const;

public slots:
    void clear();
    void setExchangeRate(double getExchangeRate);
    void changeItemCount(int id, double ile);

protected:
    QList<Merchandise*> m_list;

    bool currencyIsPLN;
    double exchangeRate;

    double przeliczSume() const;
};

#endif // TOWARMODEL_H
