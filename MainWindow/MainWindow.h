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

#include <QMessageBox>
#include <QMainWindow>
#include "TermItem.h"

class QCalendarWidget;
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

    bool isUiInitialized() const;

public slots:
    void remarksSlot();
    void loadOfferFromDatabase(const QString &offerId);
    void changeCurrency(bool pln);
/* refreshing gui elements */
    void updateTerms(const TermItem& term);
    void updateCustomer(const Customer &customer);

/*buttony na 1 zakladce*/
    void selectMerchandise();
    void globalDiscount();
    void removeRow();

/*buttony na 2 zakladce*/
    void selectCustomer();

    void chooseOfferTerms();
    void choosePaymentTerms();
    void chooseShippingTerms();
    void chooseShipmentTime();

    void setInquiryNumber(const QString& number);
    void setInquiryDate(const QDate& date);
    void setInquiryDate(const QString& date);
    void checkNr(bool);
    void checkData(bool);
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

    //baza
    void createCustomer();
    void editCustomer();
    void createMerchandise();
    void createTerms();

    //help
    void about();

signals:
    void remarksChanged(const QString& remarks);

protected:
    void bindOffer();
    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);
    //pomocnicze funkcje
    ///ustawia tytuł okna
    void setTitle(const QString &);
    ///odblokowanie interfejsu i inicjacja tabeli
    void uiInit();
    void setMenusEnabled(bool en);
    void uiReset();
    QMessageBox::StandardButton messageBoxSave();

private:
    Ui::MainWindow *ui;
    QCalendarWidget* m_calendarWidget;
    Offer* currentOffer;
};

#endif // MAINWINDOW_H
