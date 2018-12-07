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

#include "TermItem.hpp"
#include "Customer.hpp"
#include "User.hpp"

#include <QObject>
#include <QString>
#include <QDate>
#include <QVariantList>

struct VariantLists
{
    QVariantList seq;
    QVariantList id;
    QVariantList count;
    QVariantList disc;
};

class MerchandiseListModel;
class QTextDocument;
class QPrinter;
class Customer;
class MerchandiseListView;

class Offer : public QObject
{
    Q_OBJECT

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

    explicit Offer(QObject *parent = nullptr);
    virtual ~Offer() = default;

    static Offer* loadOffer(int offerID, QObject *parent = nullptr);
    static Offer* createOffer(QObject *parent = nullptr);

    void setGlobalDiscount(double discount);
    void removeMerchandiseRow(int row);

    QString document(const User& user) const;

    bool getPln() const;
    double getExchangeRate() const;
    QString getExchangeRateSql() const;
    VariantLists getMerchandiseAsVariantLists() const;

    QString getRemarks() const;
    QString getTermIDforDB(TermType type) const;
    QString getPrintOptionForDB(PrintOption opt) const;
    Customer getCustomer() const;
    QDate getDate() const;
    QString getSymbol() const;

    QString getInquiryNumberSql() const;
    QString getInquiryNumber() const;

    QString getInquiryDateSql() const;
    QString getInquiryDate() const;
    QString getInquiryText() const;

    QHash<TermType, TermItem> getTerms() const;
    PrintOptions getPrintOptions() const;

signals:
    void symbolChnged(const QString& symbol);

    void termsChanged(const TermItem& term);
    void customerChanged(const Customer& customer);
    void inquiryDateChanged(const QString& date);
    void inquiryNumberChanged(const QString& number);
    void currencyChanged(bool isPLN);
    void exchangeRateChanged(double exchangeRate);
    void printOptionsChanged(PrintOptions options);

public slots:
    void updateMerchandiseList(int id, double count);
    QHash<int, double> currentMerchandiseHash() const;
    void bindMerchandiseTable(MerchandiseListView* table);

//setters
    void setCustomer(const Customer &value);
    void setTerm(const TermItem &term);
    void setPln(bool value);
    void setExchangeRate(double value);
    void setRemarks(const QString &value);
    void setSymbol(const QString &value);
    void setDate(const QDate &value);
    //inquiry related
    void setInquiryNumber(const QString &value);
    void setInquiryDate(const QDate& date);
    void setInquiryDate(const QString& date);
    //print options
    void setPrintOptions(PrintOptions value);
    void setPrintSpecs(bool value);
    void setPrintRawPrice(bool value);
    void setPrintRawPricePLN(bool value);
    void setPrintDiscount(bool value);
    void setPrintPrice(bool value);
    void setPrintNumber(bool value);

protected:
    int id;
    QString symbol;
    QDate date;
    QString inquiryDate;
    QString inquiryNumber;
    PrintOptions printOptions;

    MerchandiseListModel* merchandiseList;
    Customer customer;

    QHash<TermType, TermItem> terms;
};

//Code for flags
Q_DECLARE_OPERATORS_FOR_FLAGS(Offer::PrintOptions)

#endif // OFFER_H
