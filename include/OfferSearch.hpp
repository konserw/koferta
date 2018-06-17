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
       
#ifndef OFFER_SEARCH_H
#define OFFER_SEARCH_H

#include <QWidget>
#include "OfferSearchModel.hpp"

class QModelIndex;
class QDate;
class QSqlRecord;

namespace Ui {
    class OfferSearch;
}

class OfferSearch : public QWidget
{
    Q_OBJECT
    
public:
    explicit OfferSearch(QWidget *parent = 0);
    ~OfferSearch();

    int currentOffer();

public slots:
    void select(const QModelIndex &);

signals:
    void selectionChanged(int);

private:
    int offerIDfromIndex(const QModelIndex &index);

    OfferSearchModel* model;
    Ui::OfferSearch *ui;
};

#endif // OFFER_SEARCH_H
