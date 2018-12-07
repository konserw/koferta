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

#include <QSqlTableModel>
#include "OfferSearchModel.hpp"

OfferSearchModel::OfferSearchModel(QObject *parent)
    : QIdentityProxyModel(parent)
{

    auto sourceModel = new QSqlTableModel(this);
    sourceModel->setTable("savedOffersView");
    sourceModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sourceModel->select();

    setSourceModel(sourceModel);
}

QVariant OfferSearchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
            switch(section)
            {
            case 0: return tr("Id");
            case 1: return tr("Symbol");
            case 2: return tr("Data");
            case 3: return tr("Oferent");
            case 4: return tr("Firma");
            case 5: return tr("Klient");
            default:
                return QString("Column %1").arg(section + 1);
            }
    }
    return QVariant();
}

void OfferSearchModel::filterByDate(const QDate &month)
{
    QString filter = QString("offerDate >= str_to_date('01.%1', '%d.%m.%Y') AND offerDate < str_to_date('01.%2', '%d.%m.%Y')")
                     .arg(month.toString("MM.yyyy"))
                     .arg(month.addMonths(1).toString("MM.yyyy"));
    dynamic_cast<QSqlTableModel*>(sourceModel())->setFilter(filter);
}

void OfferSearchModel::filterBySymbol(QStringView symbol)
{
    dynamic_cast<QSqlTableModel*>(sourceModel())->setFilter(QString("offerSymbol like '%%%1%'").arg(symbol));
}

void OfferSearchModel::filterByCustomer(QStringView customer)
{
    dynamic_cast<QSqlTableModel*>(sourceModel())->setFilter(QString("customerCompany like '%%%1%' OR customerName like '%%%1%'").arg(customer));
}

void OfferSearchModel::filterByAuthor(QStringView author)
{
    dynamic_cast<QSqlTableModel*>(sourceModel())->setFilter(QString("author = '%1'").arg(author));
}
