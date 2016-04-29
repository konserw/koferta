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
class QProcess;
class Offer;
class Customer;

namespace Ui {
    class MainWindow;
}

#if defined EXPORT_MAINWINDOW
 #define DLLSPEC Q_DECL_EXPORT
#else
 #define DLLSPEC Q_DECL_IMPORT
#endif

/*!
 * \brief Główne okno programu
 */
class DLLSPEC MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

    //Cucumber
    bool isOfferTableEnabled();

public slots:
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
    void setCustomer(const Customer &customer);

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

    Offer* currentOffer;

    User* m_currentUser;
    MerchandiseListModel* m_towarModel;

    void setOfferTerms(const TermItem& term);
    void setPaymentTerms(const TermItem& term);
    void setShippingTerms(const TermItem& term);
    void setShipmentTime(const TermItem& term);

    TermItem m_shippingTerm;
    TermItem m_shipmentTime;
    TermItem m_offerTerm;
    TermItem m_paymentTerm;
};

#endif // MAINWINDOW_H
