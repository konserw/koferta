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

#include "Offer.h"
#include "Customer.h"
#include "Database.h"
#include "MerchandiseListModel.h"
#include "User.h"

#include <QPrinter>
#include <QTextDocument>


Offer::Offer(QObject *parent) :
    QObject(parent),
    printOptions(Offer::printDiscount | Offer::printNumber | Offer::printPrice | Offer::printRawPrice | Offer::printSpecs)
{
    inquiryNumber = -1;
}

void Offer::setGlobalDiscount(double discount)
{
    merchandiseList->setGlobalRabat(discount);
}

void Offer::removeMerchandiseRow(int row)
{
    merchandiseList->removeRow(row);
}

void Offer::setOfferTerm(const TermItem &term)
{
    offerTerm = term;
}

void Offer::setPaymentTerm(const TermItem &term)
{
    paymentTerm = term;
}

void Offer::setShippingTerm(const TermItem &term)
{
    shippingTerm = term;
}

void Offer::setShipmentTime(const TermItem &term)
{
    shipmentTime = term;
}

void Offer::setInquiryDate(const QString &value)
{
    inquiryDate = value;
}

QString Offer::inquiryNumberSql() const
{
    if(inquiryNumber <= 0)
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
}

QString Offer::getRemarks() const
{
    return remarks;
}

void Offer::setRemarks(const QString &value)
{
    remarks = value;
}

QString Offer::inquiryDateSql() const
{
    if(inquiryDate.isNull() || inquiryDate.isEmpty())
        return QString("NULL");
    else
        return inquiryDate;
}

void Offer::setInquiryNumber(int value)
{
    inquiryNumber = value;
}

QString Offer::InquiryText() const
{
    QString s = "W odpowiedzi na zapytanie";
    if(inquiryNumber > 0)
        s += QString(" numer %1").arg(inquiryNumber);
    if(!inquiryDate.isEmpty())
        s += QString(" z dnia %1").arg(inquiryDate);
    s += " przedstawiamy ofertę na dostawę następujących produktów:";
    return s;
}

QString Offer::getDate() const
{
    return date;
}

void Offer::setDate(const QString &value)
{
    date = value;
}

bool Offer::getPln() const
{
    return pln;
}

void Offer::setPln(bool value)
{
    pln = value;
}
/*
double Offer::getExchangeRate() const
{
    return exchangeRate;
}
*/
void Offer::setExchangeRate(double value)
{
    merchandiseList->setKurs(value);
}

TermItem Offer::getOfferTerm() const
{
    return offerTerm;
}

void Offer::print(QPrinter *printer)
{
    const qreal margin = 5;
    printer->setPaperSize(QPrinter::A4);
    printer->setResolution(96);
    printer->setPageMargins(margin, margin, margin, margin, QPrinter::Millimeter);

    QTextDocument* doc = document();
    doc->setPageSize(QSizeF(printer->pageRect().size()));
    doc->print(printer);
    delete doc;
}

QTextDocument *Offer::document() const
{
    const int w = 745;                           //szerokosc szkieletu dokumentu
    const int dd = 248;
    const int dw = 140;                          //szerokosc pierwszej kolumny w szkielecie poniżej tabeli

    QString phone = User::current()->getPhone();
    if(!phone.isEmpty())
        phone = QString("\t\t\tTel.: %3 \n").arg(phone);

    QString html = QString(
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
                "%13\n"
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
                "%15"
                )
/* 1*/.arg(dd)
/* 2*/.arg(numberWithYear)
/* 3*/.arg(date)
/* 4*/.arg(customer.getFullName())
/* 5*/.arg(customer.getAddress())
/* 6*/.arg(customer.concatedName())
/* 7*/.arg(dd+50)
/* 8*/.arg(Database::mainAddress())
/* 9*/.arg(User::current()->getName())
/*10*/.arg(User::current()->getMail())
/*11*/.arg(phone)
/*12*/.arg(dd-50)
/*13*/.arg(User::current()->getAddress())
/*14*/.arg(InquiryText())
/*15*/.arg(merchandiseList->print(w, printOptions))
 //tabela
    *sDoc += m_towarModel->print(w,
                                 ui->kol_ilosc->isChecked(),
                                 ui->kol_cenaKat->isChecked(),
                                 ui->kol_cenaPln->isChecked(),
                                 ui->kol_rabat->isChecked(),
                                 ui->kol_cena->isChecked(),
                                 ui->kol_specyfikacja->isChecked()
                                 );
     *sDoc +=
            "</td></tr>\n"
            "<tr><td>\n"
            "\tPodane ceny nie zawierają podatku VAT<br>\n"
            "</td></tr>\n"
//warunki
            "<tr><td>\n"
            "\t<table cellspacing=3>\n"
            "\t<tr>\n"
            "\t\t<td width=";
    *sDoc += QString::number(dw);
    *sDoc += ">"
             "Warunki dostawy:</td>\n"
             "\t\t<td width=";
    *sDoc += QString::number(w-dw-3);
    *sDoc += ">";
    *sDoc += ui->plainTextEdit_dostawa->toPlainText();
    *sDoc += "</td>\n\t</tr>\n"
             "\t<tr>\n"
             "\t\t<td>Termin dostawy:</td>\n"
             "\t\t<td>";
    *sDoc += ui->plainTextEdit_termin->toPlainText();
    *sDoc += "</td>\n"
             "\t</tr>\n"
             "\t<tr>\n"
             "\t\t<td>Warunki plałatności:</td>\n"
             "\t\t<td>";
    *sDoc += ui->plainTextEdit_platnosc->toPlainText();
    *sDoc += "</td>\n"
             "\t</tr>\n";
    QString other = ui->plainTextEdit_uwagi->toPlainText();
    if(!other.isEmpty())
    {
        *sDoc += QString(
             "\t<tr>\n"
             "\t\t<td>Uwagi:</td>\n"
             "\t\t<td>%1</td>\n"
             "\t</tr>\n"
             ).arg(other.replace("\n", "<br />\n"));
    }
    *sDoc += "\t</table>\n"
             "</td></tr>\n"
             "<tr><td>\n"
             "\t<p>"
             "\n\t";
    *sDoc += ui->plainTextEdit_oferta->toPlainText();
    *sDoc += "<br>\n"
             "\tŁączymy pozdrowienia.\n"
             "\t</p>"
             "\t<p align=center style=\"margin-left: 500\">\n"
             "\t\tOfertę przygotował";
    if(!m_currentUser->male()) *sDoc += "a";
    *sDoc += "<br /><br /><br />\n"
            "\t\t";
    *sDoc += m_currentUser->name();
    *sDoc += "\n"
            "\t</p>\n"
            "</td></tr>\n"
            "</tbody>\n"
            "</table>\n"
            "</body>\n"
            "</html>\n";


    QTextDocument* doc = new QTextDocument();
    doc->setHtml(html);
    return doc;
}

TermItem Offer::getPaymentTerm() const
{
    return paymentTerm;
}

TermItem Offer::getShipmentTime() const
{
    return shipmentTime;
}

TermItem Offer::getShippingTerm() const
{
    return shippingTerm;
}
