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

#include "Logger.h"
#include "Database.h"
#include "LoginDialog.h"
#include "LoadDialog.h"
#include "User.h"
#include "Offer.h"

#include "AddConditionDialog.h"
#include "SettingsDialog.h"
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

MainWindow::MainWindow():
    QMainWindow(nullptr),
    ui(new Ui::MainWindow)
{
    qInstallMessageHandler(Logger::logHandler);

    qDebug() << "konstruktor mainwindow";

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

    qDebug() << "inicjalizacja zmiennych";
    m_pln = false;

    m_offerNumber = new QString;
    m_date = new QString(QDate::currentDate().toString("dd.MM.yyyy"));

    m_calendarWidget = new QCalendarWidget;
    m_client = NULL;

    m_towarModel = new MerchandiseListModel;
    ui->tableView->setModel(m_towarModel);

    qDebug() << "połaczenia sygnałów i slotów";
    /*menu:*/
    //oferta
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newOffer()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(loadOffer()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveOffer()));
    connect(ui->actionNR, SIGNAL(triggered()), this, SLOT(newOfferNumber()));
    //baza danych
    connect(ui->klientNowy, SIGNAL(triggered()), this, SLOT(dodajKlient()));
    connect(ui->klientEdycja, SIGNAL(triggered()), this, SLOT(edytujKlient()));
    connect(ui->actionNowy_Towar, SIGNAL(triggered()), this, SLOT(dodajTowar()));
    //export
    connect(ui->actionDo_PDF, SIGNAL(triggered()), this, SLOT(saveOffer()));
    connect(ui->actionDo_PDF, SIGNAL(triggered()), this, SLOT(printPdf()));
    connect(ui->actionDruk, SIGNAL(triggered()), this, SLOT(saveOffer()));
    connect(ui->actionDruk, SIGNAL(triggered()), this, SLOT(printPrev()));
    //info
    connect(ui->actionO_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionO_kOferta, SIGNAL(triggered()), this, SLOT(about()));
    //kOferta
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::databaseConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::databaseDisconnect);
    connect(ui->actionUstawienia, &QAction::triggered, this, &MainWindow::changeSettings);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

    //opcje wydruku
    connect(ui->pln, SIGNAL(toggled(bool)), this, SLOT(changeCurrency(bool)));
    connect(ui->kursSpinBox, SIGNAL(valueChanged(double)), m_towarModel, SLOT(setKurs(double)));

    //buttony w tabach
    connect(ui->addTowar, SIGNAL(clicked()), this, SLOT(selectMerchandise()));
    connect(ui->rabat, SIGNAL(clicked()), this, SLOT(globalDiscount()));
    connect(ui->delw, SIGNAL(clicked()), this, SLOT(removeRow()));

    //dodawanie opcji do kombosów
    connect(ui->actionDodaj_termin_dostawy, &QAction::triggered, this, &MainWindow::addShipmentTime);
    connect(ui->actionDodaj_warunki_oferty, &QAction::triggered, this, &MainWindow::addOfferTerms);
    connect(ui->actionDodaj_warunki_dostawy, &QAction::triggered, this, &MainWindow::addShippingTerms);
    connect(ui->actionDodaj_warunki_platnosci, &QAction::triggered, this, &MainWindow::addPaymentTerms);

    connect(ui->commandLinkButton_klient, &QCommandLinkButton::clicked, this, &MainWindow::selectCustomer);
    connect(ui->commandLinkButton_offerTerms, &QCommandLinkButton::clicked, this, &MainWindow::chooseOfferTerms);
    connect(ui->commandLinkButton_paymentTerms, &QCommandLinkButton::clicked, this, &MainWindow::choosePaymentTerms);
    connect(ui->commandLinkButton_shipingTerms, &QCommandLinkButton::clicked, this, &MainWindow::chooseShippingTerms);
    connect(ui->commandLinkButton_shipmentTime, &QCommandLinkButton::clicked, this, &MainWindow::chooseShipmentTime);

    connect(m_calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(setInquiryDate(QDate)));
    connect(ui->pushButton_zapytanieData, SIGNAL(clicked()), m_calendarWidget, SLOT(show()));
    connect(ui->lineEdit_zapytanieData, SIGNAL(textChanged(QString)), this, SLOT(setInquiryDate(QString)));
    connect(ui->lineEdit_zapytanieNr, SIGNAL(textChanged(QString)), this, SLOT(setInquiryNumber(QString)));
    connect(ui->checkBox_zapytanieData, SIGNAL(toggled(bool)), this, SLOT(checkData(bool)));
    connect(ui->checkBox_zapytanieNr, SIGNAL(toggled(bool)), this, SLOT(checkNr(bool)));

    ui->checkBox_zapytanieData->setChecked(true);
    ui->lineEdit_zapytanieData->setText(QDate::currentDate().toString("dd.MM.yyyy"));
    ui->checkBox_zapytanieNr->setChecked(false);
    ui->plainTextEdit_zapytanie->setReadOnly(true);

    setMenusEnabled(false);
}

MainWindow::~MainWindow()
{
    qDebug() << "destruktor mainwindow - start";

    delete ui;
    delete m_offerNumber;
    delete m_date;
    delete m_calendarWidget;
    delete m_client;

    qDebug() << "destruktor mainwindow - koniec";
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

    this->setTitle(NULL);
    ui->tab->setEnabled(false);
    ui->tab_2->setEnabled(false);

    ui->menuExport->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionNR->setEnabled(false);
}

void MainWindow::setOfferTerms(const TermItem &term)
{
    currentOffer->setOfferTerm(term);
    ui->plainTextEdit_oferta->setPlainText(term.longDesc());
}

void MainWindow::setPaymentTerms(const TermItem &term)
{
    currentOffer->setPaymentTerm(term);
    ui->plainTextEdit_platnosc->setPlainText(term.longDesc());
}

void MainWindow::setShippingTerms(const TermItem &term)
{
    currentOffer->setShippingTerm(term);
    ui->plainTextEdit_dostawa->setPlainText(term.longDesc());
}

void MainWindow::setShipmentTime(const TermItem &term)
{
    currentOffer->setShipmentTime(term);
    ui->plainTextEdit_termin->setPlainText(term.longDesc());
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Zapis przed zamknięciem"), tr("Zapisać ofertę przed zamknięciem?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
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
    LoginDialog *pop = new LoginDialog(this);
    if(pop->exec() == QDialog::Accepted)
    {
        delete pop;
        if(User::current() == nullptr)
        {
            Database::instance()->dropConection();
            QMessageBox::critical(this, tr("Nieprawidłowy użytkownik!"), tr("Użytkownik nie został odnaleziony w bazie danych.\nNastąpi zamknięcie aplikacji.\nProszę skontaktować się z administratorem i przesłąć log aplikacji."));
            qApp->quit();
        }
        qDebug() << "Logged in as" << User::current()->getDbName();
        setMenusEnabled(true);
    }
    else delete pop;
}

void MainWindow::databaseDisconnect()
{
    uiReset();
    setMenusEnabled(false);
    User::dropUser();
    Database::instance()->dropConection();
}

void MainWindow::changeSettings()
{
    SettingsDialog dialog;
    dialog.exec();
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

void MainWindow::setInquiryDate(const QDate &date)
{
    m_calendarWidget->close();
    setInquiryDate(date.toString("dd.MM.yyyy"));
}

void MainWindow::setInquiryDate(const QString &date)
{
    ui->lineEdit_zapytanieData->setText(date);
    currentOffer->setInquiryDate(date);
    ui->checkBox_zapytanieData->setChecked(true);
}

void MainWindow::checkNr(bool ch)
{
    ui->lineEdit_zapytanieNr->setEnabled(ch);
    if(!ch)
    {
        ui->lineEdit_zapytanieNr->clear();
        currentOffer->setInquiryNumber(QString::null);
    }
    ui->plainTextEdit_zapytanie->setPlainText(currentOffer->InquiryText());
}

void MainWindow::checkData(bool ch)
{
    ui->lineEdit_zapytanieData->setEnabled(ch);
    if(!ch)
    {
        ui->lineEdit_zapytanieData->clear();
        currentOffer->setInquiryDate(QString::null);
    }
    ui->plainTextEdit_zapytanie->setPlainText(currentOffer->InquiryText());
}

void MainWindow::changeCurrency(bool pln)
{
    ui->kursSpinBox->setEnabled(pln);
    ui->kurs_label->setEnabled(pln);
    ui->kol_cenaPln->setEnabled(pln);
    ui->kol_cenaPln->setChecked(pln);

    currentOffer->setExchangeRate(pln ? ui->kursSpinBox->value() : -1);
}

void MainWindow::selectCustomer()
{
    CustomerSelection* pop = new CustomerSelection(this);
    connect(pop, SIGNAL(selectionChanged(Customer)), this, SLOT(setCustomer(Customer)));
    pop->exec();
    delete pop;
}

void MainWindow::selectMerchandise()
{
    MerchandiseSelection* pop = new MerchandiseSelection(m_towarModel->hash(), this);
    connect(pop, &MerchandiseSelection::itemCountChanged, m_towarModel, &MerchandiseListModel::changeItemCount);
    pop->showMaximized();
    pop->exec();
    delete pop;
}

void MainWindow::setCustomer(const Customer& customer)
{
    currentOffer->setCustomer(customer);
    ui->plainTextEdit_klient->setPlainText(QString("%1\n%2").arg(customer.concatedName()).arg(customer.getFullName()));
}

void MainWindow::chooseOfferTerms()
{
    TermsChooserDialog* dlg = new TermsChooserDialog(this, Database::offerTermsModel(), tr("Wybór warunków oferty"));
    if(QDialog::Accepted == dlg->exec())
        setOfferTerms(dlg->choosenTerm());
    delete dlg;
}

void MainWindow::choosePaymentTerms()
{
    TermsChooserDialog* dlg = new TermsChooserDialog(this, Database::paymentTermsModel(), tr("Wybór warunków płatności"));
    if(QDialog::Accepted == dlg->exec())
        setPaymentTerms(dlg->choosenTerm());
    delete dlg;
}

void MainWindow::chooseShippingTerms()
{
    TermsChooserDialog* dlg = new TermsChooserDialog(this, Database::shippingTermsModel(), tr("Wybór warunków dostawy"));
    if(QDialog::Accepted == dlg->exec())
        setShippingTerms(dlg->choosenTerm());
    delete dlg;
}

void MainWindow::chooseShipmentTime()
{
    TermsChooserDialog* dlg = new TermsChooserDialog(this, Database::shipmentTimeModel(), tr("Wybór terminu dostawy"));
    if(QDialog::Accepted == dlg->exec())
        setShipmentTime(dlg->choosenTerm());
    delete dlg;
}

void MainWindow::setInquiryNumber(const QString &number)
{
    currentOffer->setInquiryNumber(number);
    ui->plainTextEdit_zapytanie->setPlainText(currentOffer->InquiryText());
}

void MainWindow::newOfferNumber()
{
    currentOffer->assignNewNumber();
    m_calendarWidget->setSelectedDate(currentOffer->getDate());
    this->setTitle(currentOffer->getNumberWithYear());
}

void MainWindow::newOffer()
{
    if(currentOffer)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Zapisać ofertę?"), tr("Zapisać ofertę przed zamknięciem?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
        if(reply == QMessageBox::Cancel)
            return;
        if(reply == QMessageBox::Yes)
            saveOffer();

        delete currentOffer;
    }

    currentOffer = new Offer(this);
    newOfferNumber();
    this->uiInit();
}

void MainWindow::uiInit()
{
    if(isUiInitialized())
        return;

    //włączenie zablokowanych części
    ui->tab->setEnabled(true);
    ui->tab_2->setEnabled(true);

    ui->menuExport->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionNR->setEnabled(true);

    //inicjacja tabelki
    for(int i=0; i < currentOffer->merchandiseListColumnCount(); i++)
        ui->tableView->setColumnWidth(i, 85);
    ui->tableView->setColumnWidth(1, 410);
    ui->tableView->setColumnWidth(0, 99);
}

void MainWindow::saveOffer()
{
    if(Database::instance()->save(*currentOffer) == false)
        QMessageBox::critical(this, tr("Błąd zapisywania"), tr("Wystąpił bład w trakcie zapisywania oferty.\nProszę spróbowac później, bądź skontaktować się z Administratorem."));
}

void MainWindow::loadOffer()
{
    LoadDialog* pop = new LoadDialog(this);
    connect(pop, &LoadDialog::offerSelected, this, &MainWindow::loadOfferFromDatabase);
    pop->exec();
    delete pop;
}
////////////////////////////////////////////////////////////////////////
void MainWindow::loadOfferFromDatabase(const QString& offerId)
{
    *m_offerNumber = offerId;

    this->uiInit();
    this->setTitle(*m_offerNumber);

    QSqlTableModel model;
    model.setTable("savedOffersFullView");
    model.setFilter(QString("number = '%1'").arg(offerId));
    model.select();

    QSqlRecord rec = model.record(0);
    *m_date = rec.value("date").toString();

    setCustomer(rec);

    if(rec.value("zapytanie_data").isNull())
        ui->checkBox_zapytanieData->setChecked(false);
    else
    {
        ui->checkBox_zapytanieData->setChecked(true);
        ui->lineEdit_zapytanieData->setText(rec.value("zapytanie_data").toString());
    }
    if(rec.value("zapytanie_nr").isNull())
        ui->checkBox_zapytanieNr->setChecked(false);
    else
    {
        ui->checkBox_zapytanieNr->setChecked(true);
        ui->lineEdit_zapytanieNr->setText(rec.value("zapytanie_nr").toString());
    }

    setShippingTerms(Database::shippingTerm(rec.value("dostawa").toInt()));
    setShipmentTime(Database::shipmentTime(rec.value("termin").toInt()));
    setPaymentTerms(Database::paymentTerm(rec.value("platnosc").toInt()));
    setOfferTerms(Database::offerTerm(rec.value("oferta").toInt()));

    ui->plainTextEdit_uwagi->setPlainText(rec.value("uwagi").toString());
    m_towarModel->loadOffer(*m_offerNumber);
}

void MainWindow::addShippingTerms()
{
    AddConditionDialog pop(TermItem::termShipping);
    pop.exec();
}

void MainWindow::addOfferTerms()
{
    AddConditionDialog pop(TermItem::termOffer);
    pop.exec();
}

void MainWindow::addShipmentTime()
{
    AddConditionDialog pop(TermItem::termShipmentTime);
    pop.exec();
}

void MainWindow::addPaymentTerms()
{
    AddConditionDialog pop(TermItem::termPayment);
    pop.exec();
}

void MainWindow::dodajKlient()
{
    CustomerNew* nowyKlient = new CustomerNew(this);
    nowyKlient->exec();
    delete nowyKlient;
}

void MainWindow::edytujKlient()
{
    CustomerEdit* okno = new CustomerEdit(this);
    okno->exec();
    delete okno;
}

void MainWindow::dodajTowar()
{
    MerchandiseNew* okno = new MerchandiseNew(this);
    okno->exec();
    delete okno;
}

void MainWindow::printPrev()
{
    m_htm = false;

    QPrinter* printer = new QPrinter;
    printer->setOutputFormat(QPrinter::NativeFormat);

    QPrintPreviewDialog* preview = new QPrintPreviewDialog(printer, this);
    preview->setWindowFlags(Qt::Window);
    connect(preview, SIGNAL(paintRequested(QPrinter *)), currentOffer, SLOT(print(QPrinter *)));
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
