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
#include "Customer.h"

#include <QObject>
#include <QString>

class MerchandiseListModel;
class QTextDocument;
class QPrinter;
class Customer;
class Database;

class Offer : public QObject
{
    Q_OBJECT
    friend class Database;

public:
    explicit Offer(QObject *parent = 0);

    void setGlobalDiscount(double discount);
    void removeMerchandiseRow(int row);

    TermItem getShippingTerm() const;
    TermItem getShipmentTime() const;
    TermItem getPaymentTerm() const;
    TermItem getOfferTerm() const;

    ///"drukowanie" dokumentu do podglądu lub pdf
    void print(QPrinter *printer);
    QTextDocument* document() const;

  //  double getExchangeRate() const;
    void setExchangeRate(double value);

    bool getPln() const;
    void setPln(bool value);

    QString getDate() const;
    void setDate(const QString &value);

    QString getRemarks() const;
    void setRemarks(const QString &value);

    void setInquiryDate(const QString &value);
    void setInquiryNumber(int value);


    Customer getCustomer() const;
    void setCustomer(const Customer &value);

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
    bool pln;
    QString inquiryDate;
    QString inquiryDateSql() const;
    int inquiryNumber;
    QString inquiryNumberSql() const;

    MerchandiseListModel* merchandiseList;
    Customer customer;

    TermItem shippingTerm;
    TermItem shipmentTime;
    TermItem offerTerm;
    TermItem paymentTerm;
    QString remarks;
};

#endif // OFFER_H
