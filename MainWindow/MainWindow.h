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
#include <QCalendarWidget>
#include <QPlainTextEdit>

#include "Offer.h"
#include "TermItem.h"
#include "Customer.h"


namespace Ui {
    class MainWindow;
}

#if defined EXPORT_MAINWINDOW
 #define DLLSPEC Q_DECL_EXPORT
#else
 #define DLLSPEC Q_DECL_IMPORT
#endif

class DLLSPEC MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

    bool isUiInitialized() const;
    Offer *getCurrentOffer() const;

public slots:
    void remarksSlot();
    void loadOfferFromDatabase(int offerID);
    void changeCurrency(bool pln);

    /* refreshing gui elements */
    void setTitle(const QString &);

    void updateTerms(const TermItem& term);
    void updateCustomer(const Customer &customer);
    void updateInquiryDate(const QString &date);
    void updateInquiryNumber(const QString &number);
    void updtaPrintOptions(Offer::PrintOptions options);

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
    //checkboxy
    void checkNr(bool);
    void checkData(bool);

/*menu*/
    //kOferta
    void databaseConnect();
    void databaseDisconnect();
    void changePassword();

    //oferta
    void newOffer();
    void saveOffer();
    void loadOffer();

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

    ///odblokowanie interfejsu i inicjacja tabeli
    void uiInit();
    void setMenusEnabled(bool en);
    void uiReset();
    QMessageBox::StandardButton messageBoxSave();

private:
    Ui::MainWindow *ui;
    QCalendarWidget* m_calendarWidget;
    Offer* currentOffer;

    QHash<TermType, QPlainTextEdit*> term_controls;
};

#endif // MAINWINDOW_H
