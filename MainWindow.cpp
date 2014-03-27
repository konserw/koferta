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
#include "Logowanie.h"
#include "LoadDialog.h"
#include "User.h"
#include "Macros.h"
#include "EdycjaKombo.h"

#include "CustomerNew.h"
#include "CustomerEdit.h"
#include "CustomerSelection.h"

#include "Merchandise.h"
#include "MerchandiseNew.h"
#include "MerchandiseListDelegate.h"
#include "MerchandiseSelection.h"
#include "MerchandiseListModel.h"
/*************************
**      GŁÓWNE OKNO     **
*************************/

MainWindow::~MainWindow()
{
    qDebug() << "destruktor mainwindow - start";

    delete ui;
    delete nr_oferty;
    delete data;
    delete calendarWidget;
    delete dostawaModel;
    delete platnoscModel;
    delete terminModel;
    delete ofertaModel;
    delete klient;

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
    uiReset();

/**
  zmienne
 **/
    qDebug() << "inicjalizacja zmiennych";

    pln = false;

    nr_oferty = new QString;
    data = new QString(QDate::currentDate().toString("dd.MM.yyyy"));

    calendarWidget = new QCalendarWidget;
    klient = NULL;

    m_towarModel = new MerchandiseListModel;
    ui->tableView->setModel(m_towarModel);
    ui->tableView->setItemDelegate(new MerchandiseListDelegate(this));
/**
 connections
**/
    qDebug() << "połaczenia sygnałów i slotów";

    /*menu:*/
    //oferta
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(nowa()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(popLoadDialog()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(zapisz()));
    connect(ui->actionNR, SIGNAL(triggered()), this, SLOT(nowyNumer()));
    //baza danych
    connect(ui->klientNowy, SIGNAL(triggered()), this, SLOT(dodajKlient()));
    connect(ui->klientEdycja, SIGNAL(triggered()), this, SLOT(edytujKlient()));
    connect(ui->actionNowy_Towar, SIGNAL(triggered()), this, SLOT(dodajTowar()));
    //export
    connect(ui->actionDo_HTML, SIGNAL(triggered()), this, SLOT(zapisz()));
    connect(ui->actionDo_HTML, SIGNAL(triggered()), this, SLOT(printHtm()));
    connect(ui->actionDo_PDF, SIGNAL(triggered()), this, SLOT(zapisz()));
    connect(ui->actionDo_PDF, SIGNAL(triggered()), this, SLOT(printPdf()));
    connect(ui->actionDruk, SIGNAL(triggered()), this, SLOT(zapisz()));
    connect(ui->actionDruk, SIGNAL(triggered()), this, SLOT(printPrev()));
    //info:
    connect(ui->actionO_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionO_kOferta, SIGNAL(triggered()), this, SLOT(about()));
    //connect
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::databaseConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::databaseDisconnect);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

    //opcje wydruku
    connect(ui->pln, SIGNAL(pressed()), this, SLOT(pln_on()));
    connect(ui->eur, SIGNAL(pressed()), this, SLOT(pln_off()));
//    connect(ui->kursSpinBox, &QDoubleSpinBox::valueChanged, m_towarModel, &MerchandiseListModel::setKurs);
    connect(ui->kursSpinBox, SIGNAL(valueChanged(double)), m_towarModel, SLOT(setKurs(double)));

    //buttony w tabach
    connect(ui->addTowar, SIGNAL(clicked()), this, SLOT(popWyborTowaru()));
    connect(ui->rabat, SIGNAL(clicked()), this, SLOT(rabat()));
    connect(ui->delw, SIGNAL(clicked()), this, SLOT(del()));

    //dodawanie opcji do kombosów
    connect(ui->actionDodaj_termin_dostawy, &QAction::triggered, this, &MainWindow::terminNew);
    connect(ui->actionDodaj_warunki_oferty, &QAction::triggered, this, &MainWindow::ofertaNew);
    connect(ui->actionDodaj_warunki_ostawy, &QAction::triggered, this, &MainWindow::dostawaNew);
    connect(ui->actionDodaj_warunki_platnosci, &QAction::triggered, this, &MainWindow::platnoscNew);

    //Pozostałe informacje - odświerzanie zawartości pól tekstowych
    connect(ui->commandLinkButton_klient, &QCommandLinkButton::clicked, this, &MainWindow::popWyborKlienta);
    connect(ui->comboBox_dostawa, SIGNAL(currentIndexChanged(int)), this, SLOT(dostawaRef(int)));
    connect(ui->comboBox_oferta, SIGNAL(currentIndexChanged(int)), this, SLOT(ofertaRef(int)));
    connect(ui->comboBox_platnosc, SIGNAL(currentIndexChanged(int)), this, SLOT(platnoscRef(int)));
    connect(ui->comboBox_termin, SIGNAL(currentIndexChanged(int)), this, SLOT(terminRef(int)));
    connect(calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(calChanged(QDate)));
    connect(ui->pushButton_zapytanieData, SIGNAL(clicked()), calendarWidget, SLOT(show()));
    connect(ui->lineEdit_zapytanieData, SIGNAL(textChanged(QString)), this, SLOT(zapytanieRef()));
    connect(ui->lineEdit_zapytanieNr, SIGNAL(textChanged(QString)), this, SLOT(zapytanieRef()));
    connect(ui->checkBox_zapytanieData, SIGNAL(toggled(bool)), this, SLOT(checkData(bool)));
    connect(ui->checkBox_zapytanieNr, SIGNAL(toggled(bool)), this, SLOT(checkNr(bool)));

/**
 Pozostałe informacje
**/
    ui->checkBox_zapytanieData->setText(tr("Data zapytania:"));
    ui->checkBox_zapytanieData->setChecked(true);
    ui->checkBox_zapytanieNr->setText(tr("Numer zapytania:"));
    ui->checkBox_zapytanieNr->setChecked(false);
    ui->plainTextEdit_zapytanie->setReadOnly(true);

    dostawaModel = nullptr;
    ui->plainTextEdit_dostawa->setReadOnly(true);
    ui->label_dostawa->setText(tr("Warunki dostawy:"));

    terminModel = nullptr;
    ui->plainTextEdit_termin->setReadOnly(true);
    ui->label_termin->setText(tr("Termin dostawy:"));

    platnoscModel = nullptr;
    ui->plainTextEdit_platnosc->setReadOnly(true);
    ui->label_platnosc->setText(tr("Warunki płatności:"));

    ofertaModel = nullptr;
    ui->plainTextEdit_oferta->setReadOnly(true);
    ui->label_oferta->setText(tr("Warunki Oferty:"));

    ui->label_uwagi->setText(tr("Uwagi:"));

    setMenusEnabled(false);
    QTimer::singleShot(10, this, SLOT(databaseConnect()));
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
    Logowanie loginWindow;

    connect(&loginWindow, &Logowanie::connectionSuccess, this, &MainWindow::connectedAs);

    try
    {
        loginWindow.exec();
    }
    catch (std::exception& e)
    {
        qCritical() << "[Logowanie] Standard exception: " << e.what();
    }
    catch(...)
    {
        qCritical() << "[Logowanie] Unknown exception";
    }
}

void MainWindow::databaseDisconnect()
{
    uiReset();
    setMenusEnabled(false);

    delete m_currentUser;
    m_currentUser = nullptr;

    delete klient;
    klient = nullptr;

    delete dostawaModel;
    dostawaModel = nullptr;
    delete terminModel;
    terminModel = nullptr;
    delete platnoscModel;
    terminModel = nullptr;
    delete ofertaModel;
    dostawaModel = nullptr;
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

void MainWindow::rabat()
{
    bool ok;
    double d = QInputDialog::getDouble(this, "Rabat", "Podaj domyślny rabat [%]:", 0, 0, 100, 2, &ok);
    if(ok)
        m_towarModel->setGlobalRabat(d);
}

void MainWindow::del()
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

void MainWindow::calChanged(const QDate &d)
{
    ui->lineEdit_zapytanieData->setText(d.toString("dd.MM.yyyy"));
    calendarWidget->close();
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

void MainWindow::popWyborKlienta()
{
    CustomerSelection* pop = new CustomerSelection(this);
    connect(pop, SIGNAL(selectionChanged(QSqlRecord)), this, SLOT(clientChanged(QSqlRecord)));
    pop->exec();
    delete pop;
}

void MainWindow::popWyborTowaru()
{
    MerchandiseSelection* pop = new MerchandiseSelection(m_towarModel->hash(), this);
    connect(pop, &MerchandiseSelection::itemCountChanged, m_towarModel, &MerchandiseListModel::changeItemCount);
//    pop->showMaximized();
    pop->exec();
    delete pop;
}

void MainWindow::clientChanged(const QSqlRecord& rec)
{
    delete klient;
    if(rec.isEmpty())
    {
        klient = NULL;
        ui->plainTextEdit_klient->clear();
    }
    else
    {
        klient = new QSqlRecord(rec);
        QString s;
        s = rec.value("tytul").toString();
        s += " ";
        s += rec.value("nazwisko").toString();
        s += "\n";
        s += rec.value("full").toString();
        ui->plainTextEdit_klient->setPlainText(s);
    }
}

void MainWindow::pln_on()
{
    ui->kursSpinBox->setEnabled(true);
    ui->kurs_label->setEnabled(true);
    m_towarModel->setKurs(ui->kursSpinBox->value());
}

void MainWindow::pln_off()
{
    ui->kursSpinBox->setEnabled(false);
    ui->kurs_label->setEnabled(false);
    m_towarModel->setKurs(-1);
}

/*************************
**      OFERTA          **
*************************/

void MainWindow::nowyNumer()
{
    QDate d = QDate::currentDate();
    *data = d.toString("dd.MM.yyyy");
    calendarWidget->setSelectedDate(d);

    *nr_oferty = QString::number(m_currentUser->nrOferty());
    nr_oferty->append("/");
    nr_oferty->append(d.toString("yyyy"));

    this->setTitle(nr_oferty);
}

void MainWindow::nowa()
{
    //czyszczenie starych danych
    clientChanged(QSqlRecord());

    this->nowyNumer();

    if(ui->tab->isEnabled()==false)
        this->init();
    else
        m_towarModel->clear();
}

void MainWindow::init()
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

void MainWindow::zapisz()
{
    QString s;

    if(klient == NULL)
    {
        QMessageBox::warning(this, "brak danych", "Aby zapisanie oferty w bazie danych było możliwe należy wybrać klienta.");
        return;
    }

    int anr = nr_oferty->split("/").first().toInt();

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

    insert_zapisane(*nr_oferty, klient->value("id").toInt(), *data, m_currentUser->uid(), zData, zNumer, ui->comboBox_dostawa->currentIndex(), ui->comboBox_termin->currentIndex(), ui->comboBox_platnosc->currentIndex(), ui->comboBox_oferta->currentIndex(), ui->plainTextEdit_uwagi->toPlainText());

    m_towarModel->save(*nr_oferty);
}

void MainWindow::popLoadDialog()
{
    LoadDialog* pop = new LoadDialog(this);
    connect(pop, &LoadDialog::offerSelected, this, &MainWindow::loadOffer);
    pop->exec();
    delete pop;
}

void MainWindow::loadOffer(const QString& offerId)
{
    *nr_oferty = offerId;

    this->init();
    this->setTitle(nr_oferty);

    QSqlTableModel model;
    model.setTable("savedOffersFullView");
    model.setFilter(QString("number = '%1'").arg(offerId));
    model.select();

    QSqlRecord rec = model.record(0);
    *data = rec.value("date").toString();

    clientChanged(rec);

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

    m_towarModel->loadOffer(*nr_oferty);
}

void MainWindow::dostawaNew()
{
    EdycjaKombo pop("dostawa");
    pop.exec();
    dostawaModel->select();
}

/*************************
**      BAZA DANYCH     **
*************************/

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

/*************************
**      WYDRUK          **
*************************/

void MainWindow::printPrev()
{
    htm = false;

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
    QString s;
    s = QFileDialog::getSaveFileName(this, "Zapis pdfa", "", "Dokument PDF (*.pdf)");
    if(s.isEmpty())return;

    QPrinter* printer = new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(s);

    htm = false;
    print(printer);

    delete printer;
}

void MainWindow::printHtm()
{
    QString s;
    s = QFileDialog::getSaveFileName(this, "Zapis do HTML", "", "Dokument HTML (*.htm)");
    if(s.isEmpty())return;

    QFile file(s);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        DEBUG << "plik " << s << " niedostępny";
        QMessageBox::warning(NULL, "error", "Nie udało się uzyskać dostępu do pliku");
        return;
    }

    htm = true;
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

    htm = false;
    QString* sDoc = new QString;
    makeDocument(sDoc);

    QFont* font = new QFont;
    font->setPointSize(10);
    font->setFamily("Arial");

    QTextDocument* doc = new QTextDocument;
    doc->setDefaultFont(*font);
    doc->setHtml(*sDoc);
    doc->setPageSize(QSizeF(printer->pageRect().size()));
    doc->print(printer);

    delete doc;
    delete font;
    delete sDoc;
}

void MainWindow::makeDocument(QString *sDoc)
{
    if(klient == NULL)
        klient = new QSqlRecord;

    const int w = 745;                           //szerokosc szkieletu dokumentu
    const int dd = 248;
    const int dw = 140;                          //szerokosc pierwszej kolumny w szkielecie poniżej tabeli

    *sDoc = QString(
                "<html>\n"
                "<head>\n"
                "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>\n"
                "<title>Oferta</title>\n"
                "</head>\n"
                "<body>\n"
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
            .arg(htm ? "logo2.png" : ":/logo2")
      //      .arg(w)
            .arg(dd)
            .arg(*nr_oferty)
            .arg(*data)
            .arg(klient->value("full").toString())
            .arg(klient->value("adres").toString())
            .arg(klient->value("tytul").toString())
            .arg(klient->value("imie").toString())
            .arg(klient->value("nazwisko").toString());

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
    *sDoc += "\t<font face=\"Arial Narrow\" size=11pt>\n";
    *sDoc += m_towarModel->print(w,
                                 ui->kol_kod->isChecked(),
                                 ui->kol_towar->isChecked(),
                                 ui->kol_ilosc->isChecked(),
                                 ui->kol_cenaKat->isChecked(),
                                 ui->kol_rabat->isChecked(),
                                 ui->kol_cena->isChecked()
                                 );
    *sDoc += "\t</font>\n"
            "</td></tr>\n"
            "<tr><td>\n"
            "\tPodane ceny nie zawierają podatku VAT<br>\n"
            "</td></tr>\n"
//warunki
            "<tr><td>\n"
            "\t<table cellspacing=3>\n"
            "\t<tr>\n\t\t<td width=";
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
             "\t</tr>\n"
             "\t<tr>\n"
             "\t\t<td>Uwagi:</td>\n"
             "\t\t<td>";
    *sDoc += ui->plainTextEdit_uwagi->toPlainText().replace("\n", "<br />\n");
    *sDoc += "</td>\n"
             "\t</tr>\n"
             "\t</table>\n"
             "</td></tr>\n";
//Pozdrowienia
    *sDoc += "<tr><td>\n"
             "\t";
    *sDoc += ui->plainTextEdit_oferta->toPlainText();
    *sDoc += "<br>\n"
             "\tŁączymy pozdrowienia.\n"
             "\t<p align = right style = \"margin-right: 100\">\n"
             "\t\tOfertę przygotował";
    if(!m_currentUser->male()) *sDoc += "a";
    *sDoc += "<br>\n"
            "\t\t";
    *sDoc += m_currentUser->name();
    *sDoc += "\n"
            "\t</p>\n"
            "</td></tr>\n"
            "</tbody>\n"
            "</table>\n"
            "</body>\n"
            "</html>\n";
}

void MainWindow::ofertaNew()
{
    EdycjaKombo pop("oferta");
    pop.exec();
    ofertaModel->select();
}

void MainWindow::terminNew()
{
    EdycjaKombo pop("termin");
    pop.exec();
    terminModel->select();
}

void MainWindow::platnoscNew()
{
    EdycjaKombo pop("platnosc");
    pop.exec();
    platnoscModel->select();
}
