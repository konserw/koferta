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

#ifndef OFFER_SEARCH_MODEL_H
#define OFFER_SEARCH_MODEL_H

#include <QIdentityProxyModel>
#include <QDate>
#include "MainwindowExport.hpp"

class MAINWINDOW_EXPORT OfferSearchModel: public QIdentityProxyModel
{
public:
    explicit OfferSearchModel(QObject* parent);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
public slots:
    void filterByDate(const QDate& date);
    void filterBySymbol(QStringView symbol);
    void filterByCustomer(QStringView customer);
    void filterByAuthor(QStringView author);
};

#endif
