#include "Offer.h"
#include <QTextDocument>

Offer::Offer(QObject *parent) : QObject(parent)
{

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
    shippingTerm = term
}

void Offer::setShipmentTime(const TermItem &term)
{
    shipmentTime = term;
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

    QTextDocument doc = document();
    doc.setPageSize(QSizeF(printer->pageRect().size()));
    doc.print(printer);
}

QTextDocument Offer::document() const
{
    const int w = 745;                           //szerokosc szkieletu dokumentu
    const int dd = 248;
    const int dw = 140;                          //szerokosc pierwszej kolumny w szkielecie poniżej tabeli

    QString html(
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
                "\t\t\t<b> %6 </b><br />\n"
                "\t\t\t%7 <br>\n"
                "\t\t\t%8 %9 %10 \n"
                "\t\t</td>\n"
                )
/* 1*/.arg(dd)
/* 2*/.arg(*m_offerNumber)
/* 3*/.arg(*m_date)
/* 4*/.arg(m_client->value("full").toString())
/* 5*/.arg(m_client->value("adres").toString())
/* 6*/.arg(m_client->value("tytul").toString())
/* 7*/.arg(m_client->value("imie").toString())
/* 8*/.arg(m_client->value("nazwisko").toString());

    *sDoc += QString(
                "\t\t<td width=%1>\n"
                "%2"
                 "\t\t\t<br />\n"
                 "\t\t\t<b> %3 </b><br />\n"
                 "\t\t\t%4<br />\n")
            .arg(dd+50)
            .arg(Database::mainAddress())
            .arg(m_currentUser->name())
            .arg(m_currentUser->mail());
    if(m_currentUser->hasPhone())
        *sDoc += QString("\t\t\tTel.: %3 \n")
                .arg(m_currentUser->phone());
    *sDoc += "\t\t</td>\n";

    *sDoc += QString(
            "\t\t<td width=%1>\n"
            "%2\n"
            "\t\t</td>\n")
        .arg(dd-50)
        .arg(m_currentUser->address());

    *sDoc +=
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
             "\t";
    *sDoc += ui->plainTextEdit_zapytanie->toPlainText();
    *sDoc += "\n"
             "</td></tr>\n"
             "<tr><td>\n";
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


    QTextDocument doc;
    doc.setHtml(html);
    return doc;
}

TermItem Offer::getPaymentTerm() const
{
    return paymentTerm;
}

void Offer::setOfferTerm(const TermItem &value)
{
    offerTerm = value;
}

TermItem Offer::getShipmentTime() const
{
    return shipmentTime;
}

TermItem Offer::getShippingTerm() const
{
    return shippingTerm;
}
