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

#include "Offer.hpp"
#include "Database.hpp"
#include "Customer.hpp"
#include "MerchandiseListModel.hpp"
#include "MerchandiseListView.hpp"

#include <QtGlobal>
#include <QHeaderView>
#include <QDebug>
#include <QPrinter>
#include <QTextDocument>


Offer::Offer(QObject *parent) :
    QObject(parent),
    merchandiseList(nullptr)
{
}

Offer *Offer::createOffer(QObject *parent)
{
    auto offer = new Offer(parent);
    offer->date = QDate::currentDate();
    offer->printOptions = (Offer::printDiscount | Offer::printNumber | Offer::printPrice | Offer::printRawPrice | Offer::printSpecs);
    offer->merchandiseList = new MerchandiseListModel(offer);
    offer->terms[TermType::remarks] = TermItem(TermType::remarks, QString::null, "Termin realizacji jest określany na podstawie stanu z dnia sporządzenia oferty i może ulec zmianie.");
    return offer;
}

Offer *Offer::loadOffer(int offerID, QObject* parent)
{
    qDebug() << "Loading offer" << offerID;
    auto rec = Database::loadOfferBasic(offerID);
    auto offer = new Offer(parent);

    offer->id = offerID; // need it?
    offer->symbol = rec.value("offerSymbol").toString();
    offer->date = rec.value("offerDate").toDate();
    offer->inquiryDate = rec.value("inquiryDate").toString();
    offer->inquiryNumber = rec.value("inquiryNumber").toString();
    if(!rec.value("customerID").isNull())
        offer->customer = Customer::fromRecord(rec);

    auto termTable = Database::getTermTable();
    for(auto it = termTable.begin(); it != termTable.end(); ++it)
    {
        auto val = rec.value(it.value());
        if(!val.isNull())
        {
            auto term = Database::getTerm(it.key(), val.toInt());
            offer->terms[term.getType()] = term;
        }
    }
    offer->terms[TermType::remarks] = TermItem(TermType::remarks, QString::null, rec.value("remarks").toString());

    Offer::PrintOptions options;
    options.setFlag(Offer::printSpecs, rec.value("bPrintSpecs").toBool());
    options.setFlag(Offer::printRawPrice, rec.value("bPrintRawPrice").toBool());
    options.setFlag(Offer::printRawPricePLN, rec.value("bPrintRawPricePLN").toBool());
    options.setFlag(Offer::printDiscount, rec.value("bPrintDiscount").toBool());
    options.setFlag(Offer::printPrice, rec.value("bPrintPrice").toBool());
    options.setFlag(Offer::printNumber, rec.value("bPrintNumber").toBool());
    offer->printOptions = options;

    offer->merchandiseList = new MerchandiseListModel(Database::loadOfferMerchandise(offerID), offer);

    //these are valid only when merchandise list is there
    QVariant exchange = rec.value("dExchangeRate");
    if(exchange.isNull())
        offer->setPln(false);
    else
    {
        offer->setPln(true);
        offer->setExchangeRate(exchange.toDouble());
    }

    qDebug() << "Done Loading!";
    return offer;
}

void Offer::setGlobalDiscount(double discount)
{
    merchandiseList->setGlobalRabat(discount);
}

void Offer::removeMerchandiseRow(int row)
{
    merchandiseList->removeRow(row);
}

void Offer::setTerm(const TermItem& term)
{
    terms[term.getType()] = term;
    emit termsChanged(term);
}

void Offer::updateMerchandiseList(int id, double count)
{
    merchandiseList->changeItemCount(id, count);
}

QHash<int, double> Offer::currentMerchandiseHash() const
{
    return merchandiseList->hash();
}

void Offer::bindMerchandiseTable(MerchandiseListView *table)
{
    table->setModel(merchandiseList);
}

QDate Offer::getDate() const
{
    return date;
}

void Offer::setInquiryDate(const QString &value)
{
    inquiryDate = value;
    emit inquiryDateChanged(value);
}

void Offer::setPrintOptions(PrintOptions value)
{
    printOptions = value;
    emit printOptionsChanged(printOptions);
}

void Offer::setPrintSpecs(bool value)
{
    printOptions.setFlag(Offer::printSpecs, value);
}

void Offer::setPrintRawPrice(bool value)
{
    printOptions.setFlag(Offer::printRawPrice, value);
}

void Offer::setPrintRawPricePLN(bool value)
{
    printOptions.setFlag(Offer::printRawPricePLN, value);
}

void Offer::setPrintDiscount(bool value)
{
    printOptions.setFlag(Offer::printDiscount, value);
}

void Offer::setPrintPrice(bool value)
{
    printOptions.setFlag(Offer::printPrice, value);
}

void Offer::setPrintNumber(bool value)
{
    printOptions.setFlag(Offer::printNumber, value);
}

Offer::PrintOptions Offer::getPrintOptions() const
{
    return printOptions;
}

QHash<TermType, TermItem> Offer::getTerms() const
{
    return terms;
}

void Offer::setDate(const QDate &value)
{
    date = value;
}

QString Offer::getSymbol() const
{
    return symbol;
}

void Offer::setSymbol(const QString &value)
{
    symbol = value;
    emit symbolChnged(symbol);
}

QString Offer::getInquiryNumberSql() const
{
    if(inquiryNumber.isEmpty() || inquiryNumber.isNull())
        return QString("NULL");
    else
        return QString("'%1'").arg(inquiryNumber);
}

Customer Offer::getCustomer() const
{
    return customer;
}

void Offer::setCustomer(const Customer &value)
{
    customer = value;
    emit customerChanged(customer);
}

void Offer::setRemarks(const QString &value)
{
    terms[TermType::remarks].setLongDesc(value);
}

QString Offer::getInquiryDateSql() const
{
    if(inquiryDate.isNull() || inquiryDate.isEmpty())
        return QString("NULL");
    else
        return QString("'%1'").arg(inquiryDate);
}

QString Offer::getInquiryDate() const
{
    return inquiryDate;
}

QString Offer::getInquiryNumber() const
{
    return inquiryNumber;
}

void Offer::setInquiryNumber(const QString &value)
{
    inquiryNumber = value;
    emit inquiryNumberChanged(value);
}

void Offer::setInquiryDate(const QDate &date)
{
    setInquiryDate(date.toString("dd.MM.yyyy"));
}

QString Offer::getInquiryText() const
{
    QString s = "W odpowiedzi na zapytanie";
    if(!(inquiryNumber.isEmpty() || inquiryNumber.isNull()))
        s += QString(" numer %1").arg(inquiryNumber);
    if(!inquiryDate.isEmpty())
        s += QString(" z dnia %1").arg(inquiryDate);
    s += " przedstawiamy ofertę na dostawę następujących produktów:";
    qDebug() << s;
    return s;
}

bool Offer::getPln() const
{
    return merchandiseList->isPLN();
}

double Offer::getExchangeRate() const
{
    return merchandiseList->getExchangeRate();
}

QString Offer::getExchangeRateSql() const
{
    if(getPln())
        return QString::number(merchandiseList->getExchangeRate());
    return QString("NULL");
}

VariantLists Offer::getMerchandiseAsVariantLists() const
{
    return merchandiseList->asVariantLists();
}

QString Offer::getRemarks() const
{
    return terms[TermType::remarks].longDesc();
}

QString Offer::getTermIDforDB(TermType type) const
{
    if(terms.contains(type))
        return QString::number(terms[type].id());
    return "NULL";
}

QString Offer::getPrintOptionForDB(Offer::PrintOption opt) const
{
    return printOptions.testFlag(opt)? "1" : "0";
}

void Offer::setPln(bool value)
{
    merchandiseList->setPLN(value);
    emit currencyChanged(value);
}

void Offer::setExchangeRate(double value)
{
    qDebug() << "set exchange rate" << value;
    merchandiseList->setExchangeRate(value);
    emit exchangeRateChanged(value);
}

QString Offer::document(const User &user) const
{
    const int w = 745;                           //szerokosc szkieletu dokumentu
    const int dd = 248;
    const int dw = 140;                          //szerokosc pierwszej kolumny w szkielecie poniżej tabeli

    QString phone = user.getPhone();
    if(!phone.isEmpty())
        phone = QString("\t\t\tTel.: %3 \n").arg(phone);
    QString escapedRemarks = getRemarks();
    escapedRemarks.replace("\n", "<br />\n");
    QString escapedAddress = Database::mainAddress();
    escapedAddress.replace("\n", "<br />\n");

    return QString(
                "<html>\n"
                "<head>\n"
                "<title>Oferta</title>\n"
                "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>\n"
                "<style>\n"
                "\tbody { font-family:Arial, Helvetica, sans-serif; font-size:14px; } \n"
                "\t.spec { font-size:12px; } \n"
                "\t.row0 { background: #efefef; } \n"
                "\t.row1 { background: #dadada; } \n"
                "</style>\n"
                "</head>\n"
                "<body>\n"
                "<table >\n"
      /*NAGŁÓWEK*/
                "<thead>\n"
                "<tr><td>\n"
                "\t<table>\n"
                "\t<tr>\n"
      //logo
                "\t\t<td colspan=2 align=left valign=bottom>\n"
                "\t\t\t<img src=:/aliaxis height=50 ><br>\n"
                "\t\t</td>\n"
                "\t\t<td align=center valign=bottom >\n"
                "\t\t\t<img src=:/fip height=50 ><br>\n"
                "\t\t</td>\n"
                "\t</tr>\n"
      //adresy itp
                "\t<tr>\n"
                "\t\t<td valign=top width=%1>\n"
                "\t\t\tOferta nr: <b> %2 </b><br />\n"
                "\t\t\tZ dnia: %3 <br />\n"
                "\t\t\tDla:<br />\n"
                "\t\t\t<b> %4 </b><br />\n"
                "\t\t\t%5 <br>\n"
                "\t\t\t%6 \n"
                "\t\t</td>\n"
                "\t\t<td width=%7>\n"
                "%8"
                "\t\t\t<br />\n"
                "\t\t\t<b> %9 </b><br />\n"
                "\t\t\t%10<br />\n"
                "%11"
                "\t\t</td>\n"
                "\t\t<td width=%12>\n"
                //TODO! adjust layout as address is no longer printed
                "\t\t</td>\n"
                "\t</tr>\n"
                "\t<tr>\n"
                "\t\t<td colspan=3>\n"
                "\t\t\t<hr width=100%>\n"
                "\t\t</td>\n"
                "\t</tr>\n"
                "\t</table>\n"
                "</td></tr>\n"
                "</thead>\n"
      /*Właściwa oferta*/
                "<tbody>\n"
                "<tr><td>\n"
                "\t%14\n"
                "</td></tr>\n"
                "<tr><td>\n"
      //tabela
                "%15"
                "</td></tr>\n"
                "<tr><td>\n"
                "\tPodane ceny nie zawierają podatku VAT<br>\n"
                "</td></tr>\n"
      //warunki
                "<tr><td>\n"
                "\t<table cellspacing=3>\n"
                "\t<tr>\n"
                "\t\t<td width=%16>"
                "Warunki dostawy:</td>\n"
                "\t\t<td width=%17>%18</td>\n\t</tr>\n"
                "\t<tr>\n"
                "\t\t<td>Termin dostawy:</td>\n"
                "\t\t<td>%19</td>\n"
                "\t</tr>\n"
                "\t<tr>\n"
                "\t\t<td>Warunki plałatności:</td>\n"
                "\t\t<td>%20</td>\n"
                "\t</tr>\n"
                "\t<tr>\n"
                "\t\t<td>Uwagi:</td>\n"
                "\t\t<td>%21</td>\n"
                "\t</tr>\n"
                "\t</table>\n"
                "</td></tr>\n"
                "<tr><td>\n"
                "\t<p>"
                "\n\t%22<br>\n"
                "<b>Zamówienia prosimy kierować na adres:</b> %25 z kopią do autora oferty.<br />\n"
                "<br />\n"
                "\tŁączymy pozdrowienia.\n"
                "\t</p>"
                "\t<p align=center style=\"margin-left: 500\">\n"
                "\t\tOfertę przygotował%23<br /><br /><br />\n"
                "\t\t%24\n"
                "\t</p>\n"
                "</td></tr>\n"
                "</tbody>\n"
                "</table>\n"
                "</body>\n"
                "</html>\n"
                )
/* 1*/.arg(dd)
/* 2*/.arg(symbol)
/* 3*/.arg(date.toString("dd.MM.yyyy"))
/* 4*/.arg(customer.getFullName())
/* 5*/.arg(customer.getHtmlAddress())
/* 6*/.arg(customer.concatedName())
/* 7*/.arg(dd+50)
/* 8*/.arg(escapedAddress)
/* 9*/.arg(user.getName())
/*10*/.arg(user.getMail())
/*11*/.arg(phone)
/*12*/.arg(dd-50)
/*14*/.arg(getInquiryText())
/*15*/.arg(merchandiseList->print(w, printOptions))
/*16*/.arg(dw)
/*17*/.arg(w-dw-3)
/*18*/.arg(terms.value(TermType::delivery, TermItem()).longDesc())
/*19*/.arg(terms.value(TermType::deliveryDate, TermItem()).longDesc())
/*20*/.arg(terms.value(TermType::billing, TermItem()).longDesc())
/*21*/.arg(escapedRemarks)
/*22*/.arg(terms.value(TermType::offer, TermItem()).longDesc())
/*23*/.arg(user.getGenderSuffix())
/*24*/.arg(user.getName())
/*25*/.arg(Database::getVar("order email"));
}
