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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Database.h"
#include "LoginDialog.h"
#include "LoadDialog.h"
#include "User.h"
#include "Macros.h"
#include "AddConditionDialog.h"
#include "SettingsDialog.h"

#include "CustomerNew.h"
#include "CustomerEdit.h"
#include "CustomerSelection.h"

#include "Merchandise.h"
#include "MerchandiseNew.h"
#include "MerchandiseListDelegate.h"
#include "MerchandiseSelection.h"
#include "MerchandiseListModel.h"

MainWindow::~MainWindow()
{
    qDebug() << "destruktor mainwindow - start";

    delete ui;
    delete m_offerNumber;
    delete m_date;
    delete m_calendarWidget;
    delete dostawaModel;
    delete platnoscModel;
    delete terminModel;
    delete ofertaModel;
    delete m_client;

    qDebug() << "destruktor mainwindow - koniec";
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

MainWindow::MainWindow():
    QMainWindow(nullptr),
    ui(new Ui::MainWindow)
{
    qDebug() << "konstruktor mainwindow";
/**
  ui
**/
    qDebug() << "setup ui";
    ui->setupUi(this);
    readSettings();
    uiReset();

/**
  zmienne
 **/
    qDebug() << "inicjalizacja zmiennych";

    m_pln = false;

    m_offerNumber = new QString;
    m_date = new QString(QDate::currentDate().toString("dd.MM.yyyy"));

    m_calendarWidget = new QCalendarWidget;
    m_client = NULL;

    m_towarModel = new MerchandiseListModel;
    ui->tableView->setModel(m_towarModel);
    ui->tableView->setItemDelegate(new MerchandiseListDelegate(this));
/**
 connections
**/
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
    connect(ui->actionDo_HTML, SIGNAL(triggered()), this, SLOT(saveOffer()));
    connect(ui->actionDo_HTML, SIGNAL(triggered()), this, SLOT(printHtm()));
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
    connect(ui->actionDodaj_termin_dostawy, &QAction::triggered, this, &MainWindow::terminNew);
    connect(ui->actionDodaj_warunki_oferty, &QAction::triggered, this, &MainWindow::ofertaNew);
    connect(ui->actionDodaj_warunki_ostawy, &QAction::triggered, this, &MainWindow::dostawaNew);
    connect(ui->actionDodaj_warunki_platnosci, &QAction::triggered, this, &MainWindow::platnoscNew);

    //Pozostałe informacje - odświerzanie zawartości pól tekstowych
    connect(ui->commandLinkButton_klient, &QCommandLinkButton::clicked, this, &MainWindow::selectClient);
    connect(ui->comboBox_dostawa, SIGNAL(currentIndexChanged(int)), this, SLOT(dostawaRef(int)));
    connect(ui->comboBox_oferta, SIGNAL(currentIndexChanged(int)), this, SLOT(ofertaRef(int)));
    connect(ui->comboBox_platnosc, SIGNAL(currentIndexChanged(int)), this, SLOT(platnoscRef(int)));
    connect(ui->comboBox_termin, SIGNAL(currentIndexChanged(int)), this, SLOT(terminRef(int)));
    connect(m_calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(setInquiryDate(QDate)));
    connect(ui->pushButton_zapytanieData, SIGNAL(clicked()), m_calendarWidget, SLOT(show()));
    connect(ui->lineEdit_zapytanieData, SIGNAL(textChanged(QString)), this, SLOT(zapytanieRef()));
    connect(ui->lineEdit_zapytanieNr, SIGNAL(textChanged(QString)), this, SLOT(zapytanieRef()));
    connect(ui->checkBox_zapytanieData, SIGNAL(toggled(bool)), this, SLOT(checkData(bool)));
    connect(ui->checkBox_zapytanieNr, SIGNAL(toggled(bool)), this, SLOT(checkNr(bool)));

/**
 Pozostałe informacje
**/
    ui->checkBox_zapytanieData->setChecked(true);
    ui->lineEdit_zapytanieData->setText(QDate::currentDate().toString("dd.MM.yyyy"));
    ui->checkBox_zapytanieNr->setChecked(false);
    ui->plainTextEdit_zapytanie->setReadOnly(true);

    dostawaModel = nullptr;
    ui->plainTextEdit_dostawa->setReadOnly(true);

    terminModel = nullptr;
    ui->plainTextEdit_termin->setReadOnly(true);

    platnoscModel = nullptr;
    ui->plainTextEdit_platnosc->setReadOnly(true);

    ofertaModel = nullptr;
    ui->plainTextEdit_oferta->setReadOnly(true);

    setMenusEnabled(false);
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

    if(!settings.value("settings set", false).toBool())
        QTimer::singleShot(10, this, SLOT(changeSettings()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
    /*
    if (userReallyWantsToQuit()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
    */
}

void MainWindow::about()
{
    static const QString aboutText = tr(
         "\tSystem wspomagający ofertowanie kOferta v. %1 beta 8\n"
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

    QMessageBox::about(this, tr("O kOferta"), aboutText.arg(VER).arg(__DATE__));
}

void MainWindow::connectedAs(const User &user)
{
    m_currentUser = new User(user);
    qDebug() << "Zalogowano jako uzytkownik " << m_currentUser->name();

    setMenusEnabled(true);

    dostawaModel = new QSqlTableModel;
    dostawaModel->setTable("dostawa");
    dostawaModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    dostawaModel->select();
    ui->comboBox_dostawa->setModel(dostawaModel);
    ui->comboBox_dostawa->setModelColumn(1);

    terminModel = new QSqlTableModel;
    terminModel->setTable("termin");
    terminModel->select();
    ui->comboBox_termin->setModel(terminModel);
    ui->comboBox_termin->setModelColumn(1);

    platnoscModel = new QSqlTableModel;
    platnoscModel->setTable("platnosc");
    platnoscModel->select();
    ui->comboBox_platnosc->setModel(platnoscModel);
    ui->comboBox_platnosc->setModelColumn(1);

    ofertaModel = new QSqlTableModel;
    ofertaModel->setTable("oferta");
    ofertaModel->select();
    ui->comboBox_oferta->setModel(ofertaModel);
    ui->comboBox_oferta->setModelColumn(1);

}

void MainWindow::databaseConnect()
{
    LoginDialog loginWindow;

    connect(&loginWindow, &LoginDialog::connectionSuccess, this, &MainWindow::connectedAs);

    loginWindow.exec();
}

void MainWindow::databaseDisconnect()
{
    uiReset();
    setMenusEnabled(false);

    delete m_currentUser;
    m_currentUser = nullptr;

    delete m_client;
    m_client = nullptr;

    delete dostawaModel;
    dostawaModel = nullptr;
    delete terminModel;
    terminModel = nullptr;
    delete platnoscModel;
    terminModel = nullptr;
    delete ofertaModel;
    dostawaModel = nullptr;
}

void MainWindow::changeSettings()
{
    SettingsDialog dialog;
    dialog.exec();
}

void MainWindow::setTitle(QString* nr)
{
    QString s = "kOferta v. ";
    s += QString::number(VER);

    if(nr != NULL)
    {
        s += " | oferta nr: ";
        s += *nr;
    }

    this->setWindowTitle(s);
}

void MainWindow::globalDiscount()
{
    bool ok;
    double d = QInputDialog::getDouble(this, "Rabat", "Podaj domyślny rabat [%]:", 0, 0, 100, 2, &ok);
    if(ok)
        m_towarModel->setGlobalRabat(d);
}

void MainWindow::removeRow()
{
    m_towarModel->removeRow(ui->tableView->currentIndex().row());
}

void MainWindow::dostawaRef(int row)
{
    ui->plainTextEdit_dostawa->setPlainText(dostawaModel->record(row).value("long").toString());
}

void MainWindow::platnoscRef(int row)
{
    ui->plainTextEdit_platnosc->setPlainText(platnoscModel->record(row).value("long").toString());
}

void MainWindow::terminRef(int row)
{
    ui->plainTextEdit_termin->setPlainText(terminModel->record(row).value("long").toString());
}

void MainWindow::ofertaRef(int row)
{
    ui->plainTextEdit_oferta->setPlainText(ofertaModel->record(row).value("long").toString());
}

void MainWindow::setInquiryDate(const QDate &d)
{
    ui->lineEdit_zapytanieData->setText(d.toString("dd.MM.yyyy"));
    m_calendarWidget->close();
    ui->checkBox_zapytanieData->setChecked(true);
}

void MainWindow::zapytanieRef()
{
    QString s;
    s = "W odpowiedzi na zapytanie";
    if(ui->checkBox_zapytanieNr->isChecked())
    {
        s += " numer ";
        s += ui->lineEdit_zapytanieNr->text();
    }
    if(ui->checkBox_zapytanieData->isChecked())
    {
        s += " z dnia ";
        s += ui->lineEdit_zapytanieData->text();
    }

    s += " przedstawiamy ofertę na dostawę następujących produktów:";
    ui->plainTextEdit_zapytanie->setPlainText(s);
}

void MainWindow::checkNr(bool ch)
{
    ui->lineEdit_zapytanieNr->setEnabled(ch);
    if(!ch)
        ui->lineEdit_zapytanieNr->clear();
    this->zapytanieRef();
}

void MainWindow::checkData(bool ch)
{
    ui->lineEdit_zapytanieData->setEnabled(ch);
    if(!ch)
        ui->lineEdit_zapytanieData->clear();
    this->zapytanieRef();
}

void MainWindow::changeCurrency(bool pln)
{
    ui->kursSpinBox->setEnabled(pln);
    ui->kurs_label->setEnabled(pln);
    ui->kol_cenaPln->setEnabled(pln);
    ui->kol_cenaPln->setChecked(pln);

    m_towarModel->setKurs(pln ? ui->kursSpinBox->value() : -1);
}

void MainWindow::selectClient()
{
    CustomerSelection* pop = new CustomerSelection(this);
    connect(pop, SIGNAL(selectionChanged(QSqlRecord)), this, SLOT(setClient(QSqlRecord)));
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

void MainWindow::setClient(const QSqlRecord& rec)
{
    delete m_client;
    if(rec.isEmpty())
    {
        m_client = NULL;
        ui->plainTextEdit_klient->clear();
    }
    else
    {
        m_client = new QSqlRecord(rec);
        ui->plainTextEdit_klient->setPlainText(QString("%1 %2\n%3")
                                               .arg(rec.value("tytul").toString())
                                               .arg(rec.value("nazwisko").toString())
                                               .arg(rec.value("full").toString())
                                               );
    }
}

void MainWindow::newOfferNumber()
{
    QDate d = QDate::currentDate();
    *m_date = d.toString("dd.MM.yyyy");
    m_calendarWidget->setSelectedDate(d);

    *m_offerNumber = QString::number(m_currentUser->nrOferty());
    m_offerNumber->append("/");
    m_offerNumber->append(d.toString("yyyy"));

    this->setTitle(m_offerNumber);
}

void MainWindow::newOffer()
{
    //czyszczenie starych danych
    setClient(QSqlRecord());

    this->newOfferNumber();

    if(ui->tab->isEnabled()==false)
        this->uiInit();
    else
        m_towarModel->clear();
}

void MainWindow::uiInit()
{
    //włączenie zablokowanych części
    ui->tab->setEnabled(true);
    ui->tab_2->setEnabled(true);

    ui->menuExport->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionNR->setEnabled(true);

    //inicjacja tabelki
  //  m_towarModel->clear(); //chyba niepotrzebne
    for(int i=0; i < m_towarModel->columnCount(); i++)
        ui->tableView->setColumnWidth(i, 85);
    ui->tableView->setColumnWidth(1, 410);
    ui->tableView->setColumnWidth(0, 99);
  //  ui->tableView->setSortingEnabled(true);
}

void MainWindow::saveOffer()
{
    if(m_client == nullptr)
    {
        QMessageBox::warning(this, "brak danych", "Aby zapisanie oferty w bazie danych było możliwe należy wybrać klienta.");
        return;
    }

    int anr = m_offerNumber->split("/").first().toInt();

    if(anr == m_currentUser->nrOferty())
        m_currentUser->nrOfertyInkrement();

    QString zData;
    if(ui->checkBox_zapytanieData->isChecked())
        zData = ui->lineEdit_zapytanieData->text();
    else zData = QString::null;

    QString zNumer;
    if(ui->checkBox_zapytanieNr->isChecked())
        zNumer =  ui->lineEdit_zapytanieNr->text();
    else
        zNumer = QString::null;

    insert_zapisane(*m_offerNumber, m_client->value("id").toInt(), *m_date, m_currentUser->uid(), zData, zNumer, ui->comboBox_dostawa->currentIndex(), ui->comboBox_termin->currentIndex(), ui->comboBox_platnosc->currentIndex(), ui->comboBox_oferta->currentIndex(), ui->plainTextEdit_uwagi->toPlainText());

    m_towarModel->save(*m_offerNumber);
}

void MainWindow::loadOffer()
{
    LoadDialog* pop = new LoadDialog(this);
    connect(pop, &LoadDialog::offerSelected, this, &MainWindow::loadOfferFromDatabase);
    pop->exec();
    delete pop;
}

void MainWindow::loadOfferFromDatabase(const QString& offerId)
{
    *m_offerNumber = offerId;

    this->uiInit();
    this->setTitle(m_offerNumber);

    QSqlTableModel model;
    model.setTable("savedOffersFullView");
    model.setFilter(QString("number = '%1'").arg(offerId));
    model.select();

    QSqlRecord rec = model.record(0);
    *m_date = rec.value("date").toString();

    setClient(rec);

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

    ui->comboBox_dostawa->setCurrentIndex(rec.value("dostawa").toInt());
    ui->comboBox_termin->setCurrentIndex(rec.value("termin").toInt());
    ui->comboBox_platnosc->setCurrentIndex(rec.value("platnosc").toInt());
    ui->comboBox_oferta->setCurrentIndex(rec.value("platnosc").toInt());
    ui->plainTextEdit_uwagi->setPlainText(rec.value("uwagi").toString());

    m_towarModel->loadOffer(*m_offerNumber);
}

void MainWindow::dostawaNew()
{
    AddConditionDialog pop("dostawa");
    pop.exec();
    dostawaModel->select();
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
    connect(preview, SIGNAL(paintRequested(QPrinter *)), SLOT(print(QPrinter *))); //????
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

    filePath = QFileDialog::getSaveFileName(this, "Zapis pdfa", dir, "Dokument PDF (*.pdf)");
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

    m_htm = false;
    print(printer);

    delete printer;
}

void MainWindow::printHtm()
{
    QString filePath;
    QString dir;

    QSettings settings;
    settings.beginGroup("offer");
    dir = settings.value("html directory", qApp->applicationDirPath()).toString();
    settings.endGroup();

    filePath = QFileDialog::getSaveFileName(this, "Zapis do HTML", "", "Dokument HTML (*.html)");
    if(filePath.isEmpty())return;

    if(QFileInfo(filePath).absolutePath() != dir)
    {
        QSettings settings;
        settings.beginGroup("offer");
        settings.setValue("html directory", QFileInfo(filePath).absolutePath());
        settings.endGroup();
    }

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        DEBUG << "plik " << filePath << " niedostępny";
        QMessageBox::warning(NULL, "error", "Nie udało się uzyskać dostępu do pliku");
        return;
    }

    m_htm = true;
    QString* sDoc = new QString;
    makeDocument(sDoc);

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << *sDoc;

    delete sDoc;
}

void MainWindow::print(QPrinter *printer)
{
    const qreal margin = 5;                        //szerokość marginesu
    printer->setPaperSize(QPrinter::A4);
    printer->setResolution(96);
    printer->setPageMargins(margin, margin, margin, margin, QPrinter::Millimeter);

    m_htm = false;
    QString* sDoc = new QString;
    makeDocument(sDoc);

    QTextDocument* doc = new QTextDocument;
    doc->setHtml(*sDoc);
    doc->setPageSize(QSizeF(printer->pageRect().size()));
    doc->print(printer);

    delete doc;
    delete sDoc;
}

void MainWindow::makeDocument(QString *sDoc)
{
    if(m_client == nullptr)
        m_client = new QSqlRecord;

    const int w = 745;                           //szerokosc szkieletu dokumentu
    const int dd = 248;
    const int dw = 140;                          //szerokosc pierwszej kolumny w szkielecie poniżej tabeli

    *sDoc = QString(
                "<html>\n"
                "<head>\n"
                "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>\n"
                "<title>Oferta</title>\n"
                "</head>\n"
                "<body style=\"font-family:Arial, Helvetica, sans-serif; font-size:14px;\">\n"
                "<table >\n" //cellspacing=1
    /*NAGŁÓWEK*/
                "<thead>\n"
                "<tr><td>\n"
                "\t<table>\n"
                "\t<tr>\n"
   //logo
                "\t\t<td colspan=3>\n"
                "\t\t\t<img src=%1 halign=left valign=top ><br>\n" //width=%2
                "\t\t</td>\n"
                "\t</tr>\n"
    //adresy itp
                "\t<tr>\n"
                "\t\t<td valign=top width=%3>\n"
                "\t\t\tOferta nr: <b> %4 </b><br />\n"
                "\t\t\tZ dnia: %5 <br />\n"
                "\t\t\tDla:<br />\n"
                "\t\t\t<b> %6 </b><br />\n"
                "\t\t\t%7 <br>\n"
                "\t\t\t%8 %9 %10 \n"
                "\t\t</td>\n"
                )
            .arg(m_htm ? "logo2.png" : ":/logo2")
      //      .arg(w)
            .arg(dd)
            .arg(*m_offerNumber)
            .arg(*m_date)
            .arg(m_client->value("full").toString())
            .arg(m_client->value("adres").toString())
            .arg(m_client->value("tytul").toString())
            .arg(m_client->value("imie").toString())
            .arg(m_client->value("nazwisko").toString());

    *sDoc += QString(
                "\t\t<td width=%1>\n"
                 "\t\t\t<b>Marley Polska Sp. z o.o.</b><br />\n"
                 "\t\t\tul. Dąbrówki 6<br />\n"
                 "\t\t\t03-909 Warszawa<br />\n"
                 "\t\t\t<br />\n"
                 "\t\t\t<b> %2 </b><br />\n"
                 "\t\t\t%3<br />\n")
            .arg(dd)
            .arg(m_currentUser->name())
            .arg(m_currentUser->mail());
    if(m_currentUser->hasPhone())
        *sDoc += QString("\t\t\tTel.: %3 \n")
                .arg(m_currentUser->phone());

    *sDoc += QString(
                "\t\t</td>\n"
                "\t\t<td width=%1>\n"
                "%2\n"
                "\t\t</td>\n"
           )
            .arg(dd)
            .arg(m_currentUser->adress());

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

    if(m_client->isEmpty())
    {
        delete m_client;
        m_client = nullptr;
    }
}

void MainWindow::ofertaNew()
{
    AddConditionDialog pop("oferta");
    pop.exec();
    ofertaModel->select();
}

void MainWindow::terminNew()
{
    AddConditionDialog pop("termin");
    pop.exec();
    terminModel->select();
}

void MainWindow::platnoscNew()
{
    AddConditionDialog pop("platnosc");
    pop.exec();
    platnoscModel->select();
}
