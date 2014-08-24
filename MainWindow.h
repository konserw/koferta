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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TermItem.h"

class QSqlTableModel;
class QModelIndex;
class QTextDocument;
class QTableWidgetItem;
class User;
class QSqlRecord;
class QCalendarWidget;
class QDate;
class QPrinter;
class MerchandiseListModel;

namespace Ui {
    class MainWindow;
}
/*!
 * \brief Główne okno programu
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

public slots:
    void connectedAs(const User& user);
    void loadOfferFromDatabase(const QString &offerId);

    void zapytanieRef();

    void setInquiryDate(const QDate&);
    void checkNr(bool);
    void checkData(bool);

    //opcje wydruku
    void changeCurrency(bool pln);
    ///"drukowanie" dokumentu do podglądu lub pdf
    void print(QPrinter *printer);
    ///tworzy dokument jako kod htm do QTextDocument
    void makeDocument(QString *sDoc);

/*buttony na 1 zakladce*/
    void selectMerchandise();
    void globalDiscount();
    void removeRow();

/*buttony na 2 zakladce*/
    void selectClient();
    void setClient(const QSqlRecord&);

    void chooseOfferTerms();
    void choosePaymentTerms();
    void chooseShippingTerms();
    void chooseShipmentTime();

/*menu*/
    //kOferta
    void databaseConnect();
    void databaseDisconnect();
    void changeSettings();

    //oferta
    void newOffer();
    void saveOffer();
    void loadOffer();
    void newOfferNumber();

    //export
    void printPrev();
    void printPdf();
    void printHtm();

    //baza
    void dodajKlient();
    void edytujKlient();
    void dodajTowar();
    //dodawanie opcji do kombosów
    void addShippingTerms();
    void addPaymentTerms();
    void addShipmentTime();
    void addOfferTerms();

    //help
    void about();

protected:
    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    //pomocnicze funkcje
    ///ustawia tytuł okna
    void setTitle(QString*);
    ///odblokowanie interfejsu i inicjacja tabeli
    void uiInit();
    void setMenusEnabled(bool en);
    void uiReset();

    //wewnętrzne zmienne
    QString* m_offerNumber;
    QString* m_date;
    double m_kurs;
    bool m_pln;
    bool m_htm;

    QCalendarWidget* m_calendarWidget;
    QSqlRecord* m_client;

    User* m_currentUser;
    MerchandiseListModel* m_towarModel;

    void setOfferTerms(TermItem term);
    void setPaymentTerms(TermItem term);
    void setShippingTerms(TermItem term);
    void setShipmentTime(TermItem term);

    TermItem m_shippingTerm;
    TermItem m_shipmentTime;
    TermItem m_offerTerm;
    TermItem m_paymentTerm;
};

#endif // MAINWINDOW_H
