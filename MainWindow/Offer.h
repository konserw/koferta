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

#ifndef OFFER_H
#define OFFER_H

#include "TermItem.h"
#include <QObject>
#include <QString>

class MerchandiseListModel;
class QTextDocument;
class QPrinter;
class Client;

class Offer : public QObject
{
    Q_OBJECT
public:
    explicit Offer(QObject *parent = 0);

    TermItem getShippingTerm() const;
    TermItem getShipmentTime() const;
    TermItem getPaymentTerm() const;
    TermItem getOfferTerm() const;

    ///"drukowanie" dokumentu do podglądu lub pdf
    void print(QPrinter *printer);
    QTextDocument* document() const;

signals:

public slots:
    void setOfferTerm(const TermItem& term);
    void setPaymentTerm(const TermItem& term);
    void setShippingTerm(const TermItem& term);
    void setShipmentTime(const TermItem& term);

protected:
    int number;
    QString numberWithYear;
    QString date;
    double exchangeRate;
    bool pln;

    MerchandiseListModel* merchandiseList;
    Client* client;

    TermItem shippingTerm;
    TermItem shipmentTime;
    TermItem offerTerm;
    TermItem paymentTerm;
};

#endif // OFFER_H
