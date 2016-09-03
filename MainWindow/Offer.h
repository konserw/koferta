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
#include <QDate>

class MerchandiseListModel;
class QTextDocument;
class QPrinter;
class Customer;
class Database;
class MerchandiseListView;

class Offer : public QObject
{
    Q_OBJECT
    friend class Database;

public:
    enum PrintOption : int
    {
        noPrint			= 0,
        printSpecs		= 1 << 0,
        printRawPrice	= 1 << 1,
        printRawPricePLN = 1 << 2,
        printDiscount  	= 1 << 3,
        printPrice    	= 1 << 4,
        printNumber 	= 1 << 5
    };
    Q_DECLARE_FLAGS(PrintOptions, PrintOption)

    explicit Offer(QObject *parent = 0);
    ~Offer();

    void setGlobalDiscount(double discount);
    void removeMerchandiseRow(int row);

    int merchandiseListColumnCount() const;
    bool save() const;

    TermItem getShippingTerm() const;
    TermItem getShipmentTime() const;
    TermItem getPaymentTerm() const;
    TermItem getOfferTerm() const;

    QString document() const;

  //  double getExchangeRate() const;
    void setExchangeRate(double value);

    bool getPln() const;
    void setPln(bool value);

    QString getRemarks() const;

    Customer getCustomer() const;
    void setCustomer(const Customer &value);

    QString getInquiryNumberSql() const;
    QString getInquiryNumber() const;


    QString getInquiryDateSql() const;
    QString getInquiryDate() const;
    void setInquiryDate(const QString &value);

    QString getInquiryText() const;

    QString getNumberWithYear() const;
    int getNumber() const;
    QDate getDate() const;

signals:
    void numberChnged(const QString& numberWithYear);
    void dateChanged(const QDate& date);

    void termsChanged(const TermItem& term);
    void customerChanged(const Customer& customer);
    void inquiryDateChanged(const QString& date);
    void inquiryNumberChanged(const QString& number);


public slots:
    ///"drukowanie" dokumentu do podglądu lub pdf
    void print(QPrinter *printer);

    void updateMerchandiseList(int id, double count);
    QHash<int, double> currentMerchandiseHash() const;
    void bindMerchandiseTable(MerchandiseListView* table);

    void setTerm(const TermItem &term);
    void setRemarks(const QString &value);
    void assignNewNumber();
    void setInquiryNumber(const QString &value);

protected:
    int number;
    QString numberWithYear;
    QDate date;
    bool pln;
    QString inquiryDate;
    QString inquiryNumber;
    PrintOptions printOptions;

    MerchandiseListModel* merchandiseList;
    Customer customer;

    TermItem shippingTerm;
    TermItem shipmentTime;
    TermItem offerTerm;
    TermItem paymentTerm;
    QString remarks;
};

//Code for flags
Q_DECLARE_OPERATORS_FOR_FLAGS(Offer::PrintOptions)

#endif // OFFER_H
