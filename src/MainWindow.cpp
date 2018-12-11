/**
    kOferta - system usprawniajacy proces ofertowania
    Copyright (C) 2011  Kamil "konserw" Strzempowicz, konserw@gmail.com

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


#include <QMessageBox>
#include <QDate>
#include <QInputDialog>
#include <QList>
#include <QtWidgets>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QTextDocument>
#include <QtPrintSupport>
#include <QPrintDialog>
#include <QTimer>
#include <QTranslator>

#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include "Database.hpp"
#include "DatabaseHelpers.hpp"
#include "LoginDialog.hpp"
#include "LoadDialog.hpp"
#include "Offer.hpp"

#include "AddConditionDialog.hpp"
#include "TermsChooserDialog.hpp"

#include "Customer.hpp"
#include "CustomerNew.hpp"
#include "CustomerEdit.hpp"
#include "CustomerSelection.hpp"

#include "Merchandise.hpp"
#include "MerchandiseNew.hpp"
#include "MerchandiseSelection.hpp"
#include "MerchandiseListModel.hpp"
#include "MerchandiseListView.hpp"

MainWindow::~MainWindow()
{
    qDebug() << "destruktor mainwindow - start";

    delete ui;
    delete m_calendarWidget;
    delete currentOffer;

    qDebug() << "destruktor mainwindow - koniec";
}

MainWindow::MainWindow():
    QMainWindow(nullptr),
    ui(new Ui::MainWindow)
{
    qDebug() << "konstruktor mainwindow";
    currentOffer = nullptr;

    qDebug() << "loading translation files";
    QString translationFile = QString("kOferta_%1").arg(QLocale::system().name());
    QTranslator myappTranslator;
    if(myappTranslator.load(translationFile))
    {
        qApp->installTranslator(&myappTranslator);
        qDebug() << "loaded translations from file" << translationFile;
    }
    else
        qWarning() << "\t\tCould not load translations from file" << translationFile;

    qDebug() << "setup ui";
    ui->setupUi(this);
    readSettings();
    uiReset();
    m_calendarWidget = new QCalendarWidget;

    term_controls =
    {
        {TermType::billing, ui->plainTextEdit_platnosc},
        {TermType::delivery, ui->plainTextEdit_dostawa},
        {TermType::deliveryDate, ui->plainTextEdit_termin},
        {TermType::offer, ui->plainTextEdit_oferta},
        {TermType::remarks, ui->plainTextEdit_uwagi}
    };

    qDebug() << "połaczenia sygnałów i slotów";
    /*menu:*/
    //kOferta
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::databaseConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::databaseDisconnect);
    connect(ui->action_changePassword, &QAction::triggered, this, &MainWindow::changePassword);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);
    //oferta
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newOffer);
    connect(ui->actionNR, &QAction::triggered, this, &MainWindow::assignNewSymbol);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::loadOffer);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveOffer);
    //baza danych
    connect(ui->klientNowy, SIGNAL(triggered()), this, SLOT(createCustomer()));
    connect(ui->klientEdycja, SIGNAL(triggered()), this, SLOT(editCustomer()));
    connect(ui->actionNowy_Towar, SIGNAL(triggered()), this, SLOT(createMerchandise()));
    connect(ui->actionDodaj_warunki, &QAction::triggered, this, &MainWindow::createTerms);
    //export
    connect(ui->actionDo_PDF, SIGNAL(triggered()), this, SLOT(printPdf()));
    connect(ui->actionDruk, SIGNAL(triggered()), this, SLOT(printPrev()));
    //info
    connect(ui->actionO_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionO_kOferta, SIGNAL(triggered()), this, SLOT(about()));

    //buttony w tabach
    connect(ui->addTowar, SIGNAL(clicked()), this, SLOT(selectMerchandise()));
    connect(ui->rabat, SIGNAL(clicked()), this, SLOT(globalDiscount()));
    connect(ui->delw, SIGNAL(clicked()), this, SLOT(removeRow()));

    connect(ui->commandLinkButton_klient, &QCommandLinkButton::clicked, this, &MainWindow::selectCustomer);
    connect(ui->commandLinkButton_offerTerms, &QCommandLinkButton::clicked, this, &MainWindow::chooseOfferTerms);
    connect(ui->commandLinkButton_paymentTerms, &QCommandLinkButton::clicked, this, &MainWindow::choosePaymentTerms);
    connect(ui->commandLinkButton_shipingTerms, &QCommandLinkButton::clicked, this, &MainWindow::chooseShippingTerms);
    connect(ui->commandLinkButton_shipmentTime, &QCommandLinkButton::clicked, this, &MainWindow::chooseShipmentTime);

    connect(ui->pushButton_zapytanieData, SIGNAL(clicked()), m_calendarWidget, SLOT(show()));
    connect(ui->checkBox_zapytanieData, SIGNAL(toggled(bool)), this, SLOT(checkData(bool)));
    connect(ui->checkBox_zapytanieNr, SIGNAL(toggled(bool)), this, SLOT(checkNr(bool)));
    connect(ui->plainTextEdit_uwagi, &QPlainTextEdit::textChanged, this, &MainWindow::remarksSlot);

    setMenusEnabled(false);
}

void MainWindow::setMenusEnabled(bool en)
{
    ui->menuOferta->setEnabled(en);
    ui->menuKlient->setEnabled(en);
    ui->actionDisconnect->setEnabled(en);
    ui->actionConnect->setEnabled(!en);
    ui->action_changePassword->setEnabled(en);
}

void MainWindow::uiReset()
{
    ui->tabWidget->setCurrentIndex(0);

    setTitle(QString::null);
    ui->tab->setEnabled(false);
    ui->tab_2->setEnabled(false);

    ui->plainTextEdit_klient->clear();

    for(auto* control : term_controls.values())
        control->clear();

    ui->checkBox_zapytanieData->setChecked(false);
    ui->checkBox_zapytanieNr->setChecked(false);
    ui->lineEdit_zapytanieData->clear();
    ui->lineEdit_zapytanieNr->clear();
    ui->plainTextEdit_zapytanie->clear();

    ui->radioButton_EUR->setChecked(true);
    this->changeCurrency(false);

    ui->menuExport->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionNR->setEnabled(false);
}

void MainWindow::uiInit()
{
    //enable disabled controls
    ui->tab->setEnabled(true);
    ui->tab_2->setEnabled(true);

    ui->menuExport->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionNR->setEnabled(true);

    setTitle(currentOffer->getSymbol());
    auto terms = currentOffer->getTerms();
    foreach (auto& term, terms.values()) {
        updateTerms(term);
    }
    updateCustomer(currentOffer->getCustomer());
    bool pln = currentOffer->getPln();
    if (pln)
        ui->spinBox_exchangeRate->setValue(currentOffer->getExchangeRate());
    setupCurrencyUi(pln);
    updateInquiryDate(currentOffer->getInquiryDate());
    updateInquiryNumber(currentOffer->getInquiryNumber());
    updatePrintOptions(currentOffer->getPrintOptions());

    //bind offer and ui
//general
    connect(currentOffer, &Offer::symbolChnged, this, &MainWindow::setTitle);
//first tab
    currentOffer->bindMerchandiseTable(ui->tableView);
//second tab
    //offer -> ui
    connect(currentOffer, &Offer::termsChanged, this, &MainWindow::updateTerms);
    connect(currentOffer, &Offer::customerChanged, this, &MainWindow::updateCustomer);
    //ui->offer
    connect(this, &MainWindow::remarksChanged, currentOffer, &Offer::setRemarks);
    //exchange rate related
    //offer->ui
    connect(currentOffer, &Offer::currencyChanged, this, &MainWindow::changeCurrency);
    connect(currentOffer, &Offer::exchangeRateChanged, ui->spinBox_exchangeRate, &QDoubleSpinBox::setValue);
    //ui->offer
    connect(ui->radioButton_PLN, &QRadioButton::toggled, currentOffer, &Offer::setPln);
    connect(ui->spinBox_exchangeRate, SIGNAL(valueChanged(double)), currentOffer, SLOT(setExchangeRate(double)));
    //inquiry related
    //offer->ui
    connect(currentOffer, &Offer::inquiryDateChanged, this, &MainWindow::updateInquiryDate);
    connect(currentOffer, &Offer::inquiryNumberChanged, this, &MainWindow::updateInquiryNumber);
    //ui->offer
    connect(ui->lineEdit_zapytanieNr, SIGNAL(textChanged(QString)), currentOffer, SLOT(setInquiryNumber(QString)));
    connect(ui->lineEdit_zapytanieData, SIGNAL(textChanged(QString)), currentOffer, SLOT(setInquiryDate(QString)));
    connect(m_calendarWidget, SIGNAL(clicked(QDate)), currentOffer, SLOT(setInquiryDate(QDate)));
    connect(m_calendarWidget, SIGNAL(clicked(QDate)), m_calendarWidget, SLOT(close()));
    //print options
    //offer->ui
    connect(currentOffer, &Offer::printOptionsChanged, this, &MainWindow::updatePrintOptions);
    //ui->offer
    connect(ui->kol_specyfikacja, &QCheckBox::toggled, currentOffer, &Offer::setPrintSpecs);
    connect(ui->kol_cenaKat, &QCheckBox::toggled, currentOffer, &Offer::setPrintRawPrice);
    connect(ui->kol_cenaPln, &QCheckBox::toggled, currentOffer, &Offer::setPrintRawPricePLN);
    connect(ui->kol_rabat, &QCheckBox::toggled, currentOffer, &Offer::setPrintDiscount);
    connect(ui->kol_cena, &QCheckBox::toggled, currentOffer, &Offer::setPrintPrice);
    connect(ui->kol_ilosc, &QCheckBox::toggled, currentOffer, &Offer::setPrintNumber);
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("maximized", isMaximized());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("exchangeRate", ui->spinBox_exchangeRate->value());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    ui->spinBox_exchangeRate->setValue(settings.value("exchangeRate", 4.18453702).toDouble());
    if(settings.value("maximized", false).toBool())
        this->showMaximized();
    else
    {
        resize(settings.value("size", QSize(400, 400)).toSize());
        move(settings.value("pos", QPoint(200, 200)).toPoint());
        this->show();
    }
    settings.endGroup();

    settings.beginGroup("connection");
    if(settings.value("autoconnect", false).toBool())
        QTimer::singleShot(10, this, SLOT(databaseConnect()));
    settings.endGroup();
}

bool MainWindow::messageBoxSave()
{
    auto reply = QMessageBox::question(this, tr("Zapis przed zamknięciem"), tr("Zapisać ofertę przed zamknięciem?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
    if(reply == QMessageBox::Cancel)
        return true;
    if(reply == QMessageBox::Yes)
        saveOffer();
    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(currentOffer != nullptr)
        if(messageBoxSave())
            event->ignore();
    writeSettings();
    event->accept();
}

void MainWindow::about()
{
    static const QString aboutText = tr(
         "\tSystem wspomagający ofertowanie kOferta v. %1\n"
         "\n"
         "Projekt kOferta ma ułatwić sprzedającemu tworzenie ofert handlowych poprzez "
         "łatwy dostęp do bazy danych z klientami i produktami.\n"
         "Po wybraniu pozostałych danych do oferty, "
         "jak sposób i termin dostawy umożliwia wydruk ofery według standadowego szablonu.\n"
         "Dodatkowo w bazie danych przechowywane są utworzone do tej pory oferty, "
         "co pozwala na szybką reedycję i wydruk nowej wersji odpowiedzi na zapytanie.\n"
         "Aplikacja kliencka ma także możliwość częściowej edycji bazy danych.\n"
         "\n"
         "\tLicencja:\n"
         "kOferta - system wspomagającys ofertowanie\n"
         "Copyright (C) 2011-2016 Kamil Strzempowicz, kamil.strzempowicz@gmail.com\n"
         "\n"
         "This program is free software: you can redistribute it and/or modify "
         "it under the terms of the GNU General Public License as published by "
         "the Free Software Foundation, either version 3 of the License, or "
         "(at your option) any later version.\n"
         "\n"
         "This program is distributed in the hope that it will be useful, "
         "but WITHOUT ANY WARRANTY; without even the implied warranty of "
         "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
         "GNU General Public License for more details.\n"
         "\n"
         "You should have received a copy of the GNU General Public License "
         "along with this program.  If not, see http://www.gnu.org/licenses/.\n"
         "\n"
         "For license information of third party libraries and applications"
         "please see license files in 3rdParty direcotry."
         "\n\nBuild date: %2");

    QMessageBox::about(this, tr("O kOferta"), aboutText.arg(VERSION).arg(__DATE__));
}

void MainWindow::databaseConnect()
{
    LoginDialog pop(this);
    if(pop.exec() == QDialog::Accepted)
    {
        m_user = pop.user();
        if(m_user.shouldChangePassword())
        {
            qDebug() << "User shall update password";
            changePassword();
        }
        setMenusEnabled(true);
    }
}

void MainWindow::databaseDisconnect()
{
    uiReset();
    setMenusEnabled(false);
    Database::dropConection();
    delete currentOffer;
    currentOffer = nullptr;
}

void MainWindow::setTitle(const QString& symbol)
{
    QString s;
    if(!(symbol.isNull() || symbol.isEmpty()))
        s = tr("| oferta: %1").arg(symbol);
    setWindowTitle(QString("kOferta v. %1 %2").arg(VERSION).arg(s));
}

void MainWindow::globalDiscount()
{
    bool ok;
    double d = QInputDialog::getDouble(this, tr("Rabat"), tr("Podaj domyślny rabat [%]:"), 0, 0, 100, 2, &ok);
    if(ok)
        currentOffer->setGlobalDiscount(d);
}

void MainWindow::removeRow()
{
    currentOffer->removeMerchandiseRow(ui->tableView->currentIndex().row());
}

void MainWindow::checkNr(bool ch)
{
    ui->lineEdit_zapytanieNr->setEnabled(ch);
    if(!ch)
    {
        ui->lineEdit_zapytanieNr->clear();
        currentOffer->setInquiryNumber(QString::null);
    }
}

void MainWindow::checkData(bool ch)
{
    ui->lineEdit_zapytanieData->setEnabled(ch);
    if(!ch)
    {
        ui->lineEdit_zapytanieData->clear();
        currentOffer->setInquiryDate(QString::null);
    }
}

void MainWindow::setupCurrencyUi(bool pln) {
    ui->spinBox_exchangeRate->setEnabled(pln);
    ui->kol_cenaPln->setEnabled(pln);
    ui->kol_cenaPln->setChecked(pln);
    ui->radioButton_PLN->setChecked(pln);
}

void MainWindow::changeCurrency(bool pln)
{
    setupCurrencyUi(pln);
    if (pln) {
        qDebug() << "currency changed to PLN";
        currentOffer->setExchangeRate(ui->spinBox_exchangeRate->value());
    } else {
        qDebug() << "currency changed to EUR";
    }
}

void MainWindow::updateTerms(const TermItem &term)
{
    term_controls[term.getType()]->setPlainText(term.longDesc());
}

void MainWindow::selectCustomer()
{
    CustomerSelection pop(this);
    connect(&pop, &CustomerSelection::selectionChanged, currentOffer, &Offer::setCustomer);
    pop.exec();
}

void MainWindow::selectMerchandise()
{
    MerchandiseSelection pop(currentOffer->currentMerchandiseHash(), this);
    connect(&pop, &MerchandiseSelection::itemCountChanged, currentOffer, &Offer::updateMerchandiseList);
    pop.showMaximized();
    pop.exec();
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::updateCustomer(const Customer& customer)
{
    ui->plainTextEdit_klient->setPlainText(QString("%1\n%2").arg(customer.concatedName()).arg(customer.getFullName()));
}

void MainWindow::updateInquiryDate(const QString& date)
{
    if(date.isNull() || date.isEmpty())
        ui->checkBox_zapytanieData->setChecked(false);
    else
    {
        ui->checkBox_zapytanieData->setChecked(true);
        ui->lineEdit_zapytanieData->setText(date);
    }
    ui->plainTextEdit_zapytanie->setPlainText(currentOffer->getInquiryText());
}

void MainWindow::updateInquiryNumber(const QString& number)
{
    if(number.isNull() || number.isEmpty())
        ui->checkBox_zapytanieNr->setChecked(false);
    else
    {
        ui->checkBox_zapytanieNr->setChecked(true);
        ui->lineEdit_zapytanieNr->setText(number);
    }
    ui->plainTextEdit_zapytanie->setPlainText(currentOffer->getInquiryText());
}

void MainWindow::updatePrintOptions(Offer::PrintOptions options)
{
    ui->kol_specyfikacja->setChecked(options.testFlag(Offer::printSpecs));
    ui->kol_cenaKat->setChecked(options.testFlag(Offer::printRawPrice));
    ui->kol_cenaPln->setChecked(options.testFlag(Offer::printRawPricePLN));
    ui->kol_rabat->setChecked(options.testFlag(Offer::printDiscount));
    ui->kol_cena->setChecked(options.testFlag(Offer::printPrice));
    ui->kol_ilosc->setChecked(options.testFlag(Offer::printNumber));
}

void MainWindow::remarksSlot()
{
    emit remarksChanged(ui->plainTextEdit_uwagi->toPlainText());
}

void MainWindow::chooseOfferTerms()
{
    TermsChooserDialog dlg(this, TermType::offer);
    connect(&dlg, &TermsChooserDialog::termChoosen, currentOffer, &Offer::setTerm);
    dlg.exec();
}

void MainWindow::choosePaymentTerms()
{
    TermsChooserDialog dlg(this, TermType::billing);
    connect(&dlg, &TermsChooserDialog::termChoosen, currentOffer, &Offer::setTerm);
    dlg.exec();
}

void MainWindow::chooseShippingTerms()
{
    TermsChooserDialog dlg(this, TermType::delivery);
    connect(&dlg, &TermsChooserDialog::termChoosen, currentOffer, &Offer::setTerm);
    dlg.exec();
}

void MainWindow::chooseShipmentTime()
{
    TermsChooserDialog dlg(this, TermType::deliveryDate);
    connect(&dlg, &TermsChooserDialog::termChoosen, currentOffer, &Offer::setTerm);
    dlg.exec();
}

void MainWindow::newOffer()
{
    if(currentOffer != nullptr)
    {
        if(messageBoxSave())
            return;
        delete currentOffer;
    }
    currentOffer = Offer::createOffer(this);
    uiReset();
    uiInit();
    currentOffer->setSymbol(m_user.getNewOfferSymbol());
}

void MainWindow::assignNewSymbol()
{
    currentOffer->setDate(QDate::currentDate());
    currentOffer->setSymbol(m_user.getNewOfferSymbol());
}

void MainWindow::saveOffer()
{
    try
    {
        Database::saveOffer(*currentOffer, m_user);
    }
    catch (const DatabaseException& )
    {
        QMessageBox::critical(this, tr("Błąd zapisywania"), tr("Wystąpił bład w trakcie zapisywania oferty.\nProszę spróbowac później, bądź skontaktować się z Administratorem."));
    }
}

void MainWindow::loadOffer()
{
    if(currentOffer != nullptr)
        if(messageBoxSave())
            return;

    LoadDialog pop(this);
    if(pop.exec() == QDialog::Accepted)
        loadOfferFromDatabase(pop.selectedOfferId());
}

void MainWindow::changePassword()
{
    int uid = m_user.getUid();
    QString password = QInputDialog::getText(nullptr, tr("Zmiana hasła"), tr("Proszę wprowadzić nowe hasło"), QLineEdit::Password);
    if(password.isEmpty() || password.isNull())
    {
        qDebug() << "Skipped password change";
        return;
    }

    try
    {
        Database::setPassword(uid, password);
    }
    catch (const DatabaseException& )
    {
        qDebug() << "Some error during password change occured";
        QMessageBox::warning(nullptr, tr("Hasło nie zmienione"), tr("Wystąpił błąd podczas zmiany hasła.\nHasło nie zostało zmienione"));
        return;
    }

    qDebug() << "Password updated";
    QMessageBox::information(nullptr, tr("Hasło zmienione"), tr("Nowe hasło zostało poprawnie zapisane w baziedanych."));
}

void MainWindow::loadOfferFromDatabase(int offerID)
{
    delete currentOffer;
    uiReset();
    try
    {
        currentOffer = Offer::loadOffer(offerID, this);
    }
    catch (const DatabaseException& )
    {
        currentOffer = nullptr;
        QMessageBox::critical(this, tr("Błąd"), tr("Wystąpił błąd podczas wczytywania oferty.\nSpróbuj ponownie"));
        return;
    }
    uiInit();
}

void MainWindow::createTerms()
{
    AddConditionDialog pop(this);
    pop.exec();
}

void MainWindow::createCustomer()
{
    CustomerNew pop(this);
    pop.exec();
}

void MainWindow::editCustomer()
{
    CustomerEdit pop(this);
    pop.exec();
}

void MainWindow::createMerchandise()
{
    MerchandiseNew pop(this);
    pop.exec();
}

void MainWindow::print(QPrinter *printer)
{
    const qreal margin = 5;
    printer->setPaperSize(QPrinter::A4);
    printer->setResolution(96);
    printer->setPageMargins(margin, margin, margin, margin, QPrinter::Millimeter);

    QTextDocument doc;
    doc.setHtml(currentOffer->document(m_user));
    doc.setPageSize(QSizeF(printer->pageRect().size()));
    doc.print(printer);
}

void MainWindow::printPrev()
{
    QPrinter printer;
    printer.setOutputFormat(QPrinter::NativeFormat);

    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowFlags(Qt::Window);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &MainWindow::print);
    preview.showMaximized();
    preview.exec();
}

void MainWindow::printPdf()
{
    QString filePath;
    QString dir;

    QSettings settings;
    settings.beginGroup("offer");
    dir = settings.value("pdf directory", qApp->applicationDirPath()).toString();
    settings.endGroup();

    filePath = QFileDialog::getSaveFileName(this, tr("Zapis pdfa"), dir, tr("Dokument PDF (*.pdf)"));
    if(filePath.isEmpty())return;

    if(QFileInfo(filePath).absolutePath() != dir)
    {
        QSettings settings;
        settings.beginGroup("offer");
        settings.setValue("pdf directory", QFileInfo(filePath).absolutePath());
        settings.endGroup();
    }

    QPrinter* printer = new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(filePath);

    print(printer);

    delete printer;
}
