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

#include <QDate>
#include <QSqlQuery>
#include <QInputDialog>
#include <QList>
#include <QtWidgets>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QTextDocument>
#include <QtPrintSupport>
#include <QPrintDialog>
#include <QTimer>
#include <QTranslator>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Database.h"
#include "LoginDialog.h"
#include "LoadDialog.h"
#include "User.h"
#include "Offer.h"

#include "AddConditionDialog.h"
#include "TermsChooserDialog.h"

#include "Customer.h"
#include "CustomerNew.h"
#include "CustomerEdit.h"
#include "CustomerSelection.h"

#include "Merchandise.h"
#include "MerchandiseNew.h"
#include "MerchandiseSelection.h"
#include "MerchandiseListModel.h"
#include "MerchandiseListView.h"

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

    //m_date = new QString(QDate::currentDate().toString("dd.MM.yyyy"));
    m_calendarWidget = new QCalendarWidget;

    qDebug() << "połaczenia sygnałów i slotów";
    /*menu:*/
    //oferta
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newOffer()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(loadOffer()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveOffer()));
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
    //kOferta
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::databaseConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::databaseDisconnect);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

    //opcje wydruku ------------------------------ TODO !
  //  connect(ui->pln, SIGNAL(toggled(bool)), this, SLOT(changeCurrency(bool)));
  //  connect(ui->kursSpinBox, SIGNAL(valueChanged(double)), m_towarModel, SLOT(setKurs(double)));

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
  //  connect(ui->lineEdit_zapytanieData, SIGNAL(textChanged(QString)), this, SLOT(setInquiryDate(QString)));
    connect(ui->checkBox_zapytanieData, SIGNAL(toggled(bool)), this, SLOT(checkData(bool)));
    connect(ui->checkBox_zapytanieNr, SIGNAL(toggled(bool)), this, SLOT(checkNr(bool)));

    connect(ui->plainTextEdit_uwagi, &QPlainTextEdit::textChanged, this, &MainWindow::remarksSlot);
    connect(ui->lineEdit_zapytanieData, SIGNAL(textChanged(QString)), this, SLOT(updateInquiryText()));
    connect(ui->lineEdit_zapytanieNr, SIGNAL(textChanged(QString)), this, SLOT(updateInquiryText()));

    //TODO - check
  //  ui->checkBox_zapytanieData->setChecked(true);
  //  ui->lineEdit_zapytanieData->setText(QDate::currentDate().toString("dd.MM.yyyy"));
  //  ui->checkBox_zapytanieNr->setChecked(false);
  //  ui->plainTextEdit_zapytanie->setReadOnly(true);

    setMenusEnabled(false);
}

void MainWindow::bindOffer()
{
    //inquiry related ui->offer
    connect(ui->lineEdit_zapytanieNr, SIGNAL(textChanged(QString)), currentOffer, SLOT(setInquiryNumber(QString)));
    connect(ui->lineEdit_zapytanieData, SIGNAL(textChanged(QString)), currentOffer, SLOT(setInquiryNumber(QString)));
    connect(m_calendarWidget, SIGNAL(clicked(QDate)), currentOffer, SLOT(setInquiryDate(QDate)));

    connect(ui->actionNR, SIGNAL(triggered(bool)), currentOffer, SLOT(assignNewNumber()));
    connect(this, &MainWindow::remarksChanged, currentOffer, &Offer::setRemarks);

    connect(currentOffer, &Offer::numberChnged, this, &MainWindow::setTitle);
    connect(currentOffer, &Offer::termsChanged, this, &MainWindow::updateTerms);
    connect(currentOffer, &Offer::customerChanged, this, &MainWindow::updateCustomer);
    //inquiry related offer->ui
    connect(currentOffer, &Offer::inquiryDateChanged, this, &MainWindow::updateInquiryDate);
    connect(currentOffer, &Offer::inquiryNumberChanged, this, &MainWindow::updateInquiryNumber);
}

bool MainWindow::isUiInitialized() const
{
    return (ui->tab->isEnabled() &&
            ui->tab_2->isEnabled() &&
            ui->menuExport->isEnabled() &&
            ui->actionSave->isEnabled() &&
            ui->actionNR->isEnabled()
            );
}

void MainWindow::setMenusEnabled(bool en)
{
    ui->menuOferta->setEnabled(en);
    ui->menuKlient->setEnabled(en);
    ui->actionDisconnect->setEnabled(en);
    ui->actionConnect->setEnabled(!en);
}

void MainWindow::uiReset()
{
    ui->tabWidget->setCurrentIndex(0);

    setTitle(QString::null);
    ui->tab->setEnabled(false);
    ui->tab_2->setEnabled(false);

    ui->plainTextEdit_dostawa->clear();
    ui->plainTextEdit_klient->clear();
    ui->plainTextEdit_oferta->clear();
    ui->plainTextEdit_platnosc->clear();
    ui->plainTextEdit_termin->clear();
    ui->plainTextEdit_uwagi->clear();

    ui->checkBox_zapytanieData->setChecked(false);
    ui->checkBox_zapytanieNr->setChecked(false);
    ui->lineEdit_zapytanieData->clear();
    ui->lineEdit_zapytanieNr->clear();
    ui->plainTextEdit_zapytanie->clear();

    ui->menuExport->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionNR->setEnabled(false);
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("maximized", isMaximized());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("exchangeRate", ui->kursSpinBox->value());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    ui->kursSpinBox->setValue(settings.value("exchangeRate", 4.18453702).toDouble());
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

QMessageBox::StandardButton MainWindow::messageBoxSave()
{
    return QMessageBox::question(this, tr("Zapis przed zamknięciem"), tr("Zapisać ofertę przed zamknięciem?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
}

Offer *MainWindow::getCurrentOffer() const
{
    return currentOffer;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply;
    if(currentOffer == nullptr)
        reply = QMessageBox::No;
    else
        reply =  messageBoxSave();

    if(reply == QMessageBox::Cancel)
    {
        event->ignore();
    }
    else
    {
        if(reply == QMessageBox::Yes)
            saveOffer();
        writeSettings();
        event->accept();
    }
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
         "kOferta - system usprawniajacy proces ofertowania\n"
         "Copyright (C) 2011  Kamil 'konserw' Strzempowicz, konserw@gmail.com\n"
         "\n"
         "This program is free software: you can redistribute it and/or modify "
         "it under the terms of the GNU General Public License as published by "
         "the Free Software Foundation, either version 3 of the License, or "
         "(at your option) any later version.\n"
         "\n"
         "This program is distributed in the hope that it will be useful, "
         "but WITHOUT ANY WARRANTY; without even the implied warranty of "
         "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
         "GNU General Public License for more details.\n"
         "\n"
         "You should have received a copy of the GNU General Public License "
         "along with this program.  If not, see http://www.gnu.org/licenses/.\n"
         "\n"
         "W programie wykorzystano klasę implementującą szyfrowanie SHA1 "
         "autorstwa Michael D. Leonhard na warunkach licencyjnych opisanych w pliku SHA1_LICENSE\n"
         "\n\nBuild date: %2");

    QMessageBox::about(this, tr("O kOferta"), aboutText.arg(VERSION).arg(__DATE__));
}

void MainWindow::databaseConnect()
{
    LoginDialog* pop = new LoginDialog(this);
    if(pop->exec() == QDialog::Accepted)
    {
        if(User::current() == nullptr)
        {
            Database::instance()->dropConection();
            QMessageBox::critical(this, tr("Nieprawidłowy użytkownik!"), tr("Użytkownik nie został odnaleziony w bazie danych.\nNastąpi zamknięcie aplikacji.\nProszę skontaktować się z administratorem i przesłąć log aplikacji."));
            qApp->quit();
            return;
        }
        setMenusEnabled(true);
    }
}

void MainWindow::databaseDisconnect()
{
    uiReset();
    setMenusEnabled(false);
    User::dropUser();
    Database::instance()->dropConection();
    delete currentOffer;
    currentOffer = nullptr;
}

void MainWindow::setTitle(const QString& nr)
{
    QString s;
    if(!(nr.isNull() || nr.isEmpty()))
        s = QString("| oferta nr: %1").arg(nr);
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

void MainWindow::changeCurrency(bool pln)
{
    ui->kursSpinBox->setEnabled(pln);
    ui->kurs_label->setEnabled(pln);
    ui->kol_cenaPln->setEnabled(pln);
    ui->kol_cenaPln->setChecked(pln);

    currentOffer->setExchangeRate(pln ? ui->kursSpinBox->value() : -1);
}

void MainWindow::updateTerms(const TermItem &term)
{
    switch(term.getType())
    {
    case TermItem::termOffer:
        ui->plainTextEdit_oferta->setPlainText(term.longDesc());
        break;
    case TermItem::termPayment:
        ui->plainTextEdit_platnosc->setPlainText(term.longDesc());
        break;
    case TermItem::termShipmentTime:
        ui->plainTextEdit_termin->setPlainText(term.longDesc());
        break;
    case TermItem::termShipping:
        ui->plainTextEdit_dostawa->setPlainText(term.longDesc());
        break;
    case TermItem::termRemarks:
        ui->plainTextEdit_uwagi->setPlainText(term.longDesc());
        break;
    default:
        qCritical() << "Invalid TermItem object";
    }
}


void MainWindow::selectCustomer()
{
    CustomerSelection* pop = new CustomerSelection(this);
    connect(pop, &CustomerSelection::selectionChanged, currentOffer, &Offer::setCustomer);
    pop->exec();
}

void MainWindow::selectMerchandise()
{
    MerchandiseSelection* pop = new MerchandiseSelection(currentOffer->currentMerchandiseHash(), this);
    connect(pop, &MerchandiseSelection::itemCountChanged, currentOffer, &Offer::updateMerchandiseList);
    pop->showMaximized();
    pop->exec();
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
}

void MainWindow::updateInquiryText()
{
    ui->plainTextEdit_zapytanie->setPlainText(currentOffer->getInquiryText());
}

void MainWindow::remarksSlot()
{
    emit remarksChanged(ui->plainTextEdit_uwagi->toPlainText());
}

void MainWindow::chooseOfferTerms()
{
    TermsChooserDialog* dlg = new TermsChooserDialog(this, TermItem::termOffer);
    connect(dlg, &TermsChooserDialog::termChoosen, currentOffer, &Offer::setTerm);
    dlg->exec();
}

void MainWindow::choosePaymentTerms()
{
    TermsChooserDialog* dlg = new TermsChooserDialog(this, TermItem::termPayment);
    connect(dlg, &TermsChooserDialog::termChoosen, currentOffer, &Offer::setTerm);
    dlg->exec();
}

void MainWindow::chooseShippingTerms()
{
    TermsChooserDialog* dlg = new TermsChooserDialog(this, TermItem::termShipping);
    connect(dlg, &TermsChooserDialog::termChoosen, currentOffer, &Offer::setTerm);
    dlg->exec();
}

void MainWindow::chooseShipmentTime()
{
    TermsChooserDialog* dlg = new TermsChooserDialog(this, TermItem::termShipmentTime);
    connect(dlg, &TermsChooserDialog::termChoosen, currentOffer, &Offer::setTerm);
    dlg->exec();
}

void MainWindow::newOffer()
{
    if(currentOffer != nullptr)
    {
        QMessageBox::StandardButton reply = messageBoxSave();
        if(reply == QMessageBox::Cancel)
            return;
        if(reply == QMessageBox::Yes)
            saveOffer();

        delete currentOffer;
    }
    currentOffer = new Offer(this);
    uiReset();
    uiInit();
    bindOffer();
    currentOffer->assignNewNumber();
}

void MainWindow::uiInit()
{
    currentOffer->bindMerchandiseTable(ui->tableView);

    if(isUiInitialized())
        return;

    //włączenie zablokowanych części
    ui->tab->setEnabled(true);
    ui->tab_2->setEnabled(true);

    ui->menuExport->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionNR->setEnabled(true);
}

void MainWindow::saveOffer()
{
    if(Database::instance()->save(*currentOffer) == false)
        QMessageBox::critical(this, tr("Błąd zapisywania"), tr("Wystąpił bład w trakcie zapisywania oferty.\nProszę spróbowac później, bądź skontaktować się z Administratorem."));
}

void MainWindow::loadOffer()
{
    if(currentOffer != nullptr)
    {
        QMessageBox::StandardButton reply = messageBoxSave();
        if(reply == QMessageBox::Cancel)
            return;
        if(reply == QMessageBox::Yes)
            saveOffer();
    }
    LoadDialog* pop = new LoadDialog(this);
    connect(pop, &LoadDialog::offerSelected, this, &MainWindow::loadOfferFromDatabase);
    pop->exec();
}

void MainWindow::loadOfferFromDatabase(const QString& offerId)
{
    delete currentOffer;
    currentOffer = new Offer;
    bindOffer();
    uiReset();
    uiInit();
    setTitle(offerId);
    Database::instance()->loadOffer(currentOffer, offerId);
}

void MainWindow::createTerms()
{
    AddConditionDialog pop(this);
    pop.exec();
}

void MainWindow::createCustomer()
{
    CustomerNew* pop = new CustomerNew(this);
    pop->exec();
}

void MainWindow::editCustomer()
{
    CustomerEdit* pop = new CustomerEdit(this);
    pop->exec();
}

void MainWindow::createMerchandise()
{
    MerchandiseNew* pop = new MerchandiseNew(this);
    pop->exec();
}

void MainWindow::printPrev()
{
    QPrinter* printer = new QPrinter;
    printer->setOutputFormat(QPrinter::NativeFormat);

    QPrintPreviewDialog* preview = new QPrintPreviewDialog(printer, this);
    preview->setWindowFlags(Qt::Window);
    connect(preview, &QPrintPreviewDialog::paintRequested, currentOffer, &Offer::print);
    preview->exec();

    delete preview;
    delete printer;
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

    currentOffer->print(printer);

    delete printer;
}
