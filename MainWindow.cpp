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
#include "NowyKlient.h"
#include "Database.h"
#include "EdycjaKlienta.h"
#include "WyborKlienta.h"
#include "Logowanie.h"
#include "LoadDialog.h"
#include "User.h"
#include "Macros.h"
#include "EdycjaKombo.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"


#include "MerchandiseSelection.h"
#include "MerchandiseListModel.h"
#include "Merchandise.h"
#include "MerchandiseListDelegate.h"

/*************************
**      GŁÓWNE OKNO     **
*************************/

MainWindow::~MainWindow()
{
    DEBUG << "destruktor mainwindow - start";

    delete ui;
    delete nr_oferty;
    delete data;
    delete calendarWidget;
    delete dostawaModel;
    delete platnoscModel;
    delete terminModel;
    delete ofertaModel;
    delete klient;

    DEBUG << "destruktor mainwindow - koniec";
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
    DEBUG << "konstruktor mainwindow";   
/**
  zmienne
 **/
    DEBUG << "inicjalizacja zmiennych";

    ui->setupUi(this);
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
    DEBUG << "połaczenia sygnałów i slotów";

    /*menu:*/
    //oferta
    QObject::connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(nowa()));
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(popLoadDialog()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(zapisz()));
    QObject::connect(ui->actionNR, SIGNAL(triggered()), this, SLOT(nowyNumer()));
    //klient
    QObject::connect(ui->klientNowy, SIGNAL(triggered()), this, SLOT(dodajKlient()));
    QObject::connect(ui->klientEdycja, SIGNAL(triggered()), this, SLOT(edytujKlient()));
    //export
    QObject::connect(ui->actionDo_HTML, SIGNAL(triggered()), this, SLOT(zapisz()));
    QObject::connect(ui->actionDo_HTML, SIGNAL(triggered()), this, SLOT(printHtm()));
    QObject::connect(ui->actionDo_PDF, SIGNAL(triggered()), this, SLOT(zapisz()));
    QObject::connect(ui->actionDo_PDF, SIGNAL(triggered()), this, SLOT(printPdf()));
    QObject::connect(ui->actionDruk, SIGNAL(triggered()), this, SLOT(zapisz()));
    QObject::connect(ui->actionDruk, SIGNAL(triggered()), this, SLOT(printPrev()));
    //info:
    QObject::connect(ui->actionO_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    QObject::connect(ui->actionO_kOferta, SIGNAL(triggered()), this, SLOT(about()));
    //connect
    QObject::connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connect);
    QObject::connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnect);
    QObject::connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

    //opcje wydruku
    QObject::connect(ui->pln, SIGNAL(pressed()), this, SLOT(pln_on()));
    QObject::connect(ui->eur, SIGNAL(pressed()), this, SLOT(pln_off()));

    //buttony w tabach
    QObject::connect(ui->addTowar, SIGNAL(clicked()), this, SLOT(popWyborTowaru()));
    QObject::connect(ui->rabat, SIGNAL(clicked()), this, SLOT(rabat()));
    QObject::connect(ui->delw, SIGNAL(clicked()), this, SLOT(del()));

    //dodawanie opcji do kombosów
    QObject::connect(ui->pushButton_dostawa, SIGNAL(clicked()), this, SLOT(dostawaNew()));
    QObject::connect(ui->pushButton_oferta, SIGNAL(clicked()), this, SLOT(ofertaNew()));
    QObject::connect(ui->pushButton_platnosc, SIGNAL(clicked()), this, SLOT(platnoscNew()));
    QObject::connect(ui->pushButton_termin, SIGNAL(clicked()), this, SLOT(terminNew()));

    //Pozostałe informacje - odświerzanie zawartości pól tekstowych
    QObject::connect(ui->pushButton_wyborKlienta, SIGNAL(clicked()), this, SLOT(popWyborKlienta()));
    QObject::connect(ui->comboBox_dostawa, SIGNAL(currentIndexChanged(int)), this, SLOT(dostawaRef(int)));
    QObject::connect(ui->comboBox_oferta, SIGNAL(currentIndexChanged(int)), this, SLOT(ofertaRef(int)));
    QObject::connect(ui->comboBox_platnosc, SIGNAL(currentIndexChanged(int)), this, SLOT(platnoscRef(int)));
    QObject::connect(ui->comboBox_termin, SIGNAL(currentIndexChanged(int)), this, SLOT(terminRef(int)));
    QObject::connect(calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(calChanged(QDate)));
    QObject::connect(ui->pushButton_zapytanieData, SIGNAL(clicked()), calendarWidget, SLOT(show()));
    QObject::connect(ui->lineEdit_zapytanieData, SIGNAL(textChanged(QString)), this, SLOT(zapytanieRef()));
    QObject::connect(ui->lineEdit_zapytanieNr, SIGNAL(textChanged(QString)), this, SLOT(zapytanieRef()));
    QObject::connect(ui->checkBox_zapytanieData, SIGNAL(toggled(bool)), this, SLOT(checkData(bool)));
    QObject::connect(ui->checkBox_zapytanieNr, SIGNAL(toggled(bool)), this, SLOT(checkNr(bool)));

/**
  ui
**/
    uiReset();

    const QString addOpt = tr("Dodaj");
    ui->pushButton_dostawa->setText(addOpt);
    ui->pushButton_oferta->setText(addOpt);
    ui->pushButton_platnosc->setText(addOpt);
    ui->pushButton_termin->setText(addOpt);

/**
 Pozostałe informacje
**/

    ui->label_klient->setText(tr("Klient:"));
    ui->pushButton_wyborKlienta->setText(tr("Wybór klienta"));
    ui->plainTextEdit_klient->setReadOnly(true);

    ui->label_zapytanie->setText(tr("Zapytanie:"));
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
    QTimer::singleShot(10, this, SLOT(connect()));
}

void MainWindow::about()
{
    QString s;
    s = "System wspomagający ofertowanie kOferta v. ";
    s += QString::number(VER);
    s += "\n"
         "Głównym elementem systemu jest aplikacja kliencka stanowiąca front-end dla danych znajdujących się w bazie danych. "
         "Baza ta przechowuje nie tylko dane klientów i towarów przedsiębiorstwa, ale także utworzone do tej pory oferty. "
         "Aplikacja kliencka ma także możliwość edycji bazy danych.\n\n"
         "\t\tLicencja:\n"
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
         "\n\n\t\t\tBuild date: " __DATE__;
    QMessageBox::about(this, "O kOferta", s);
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

void MainWindow::connect()
{
    Logowanie loginWindow;

    QObject::connect(&loginWindow, &Logowanie::connectionSuccess, this, &MainWindow::connectedAs);

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

void MainWindow::disconnect()
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
    WyborKlienta* pop = new WyborKlienta(this);
    QObject::connect(pop, SIGNAL(selectionChanged(QSqlRecord)), this, SLOT(clientChanged(QSqlRecord)));
    pop->exec();
    delete pop;
}

void MainWindow::popWyborTowaru()
{
    MerchandiseSelection* pop = new MerchandiseSelection(m_towarModel->hash(), this);
    QObject::connect(pop, &MerchandiseSelection::itemCountChanged, m_towarModel, &MerchandiseListModel::changeItemCount);
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
    m_towarModel->setKurs(0);
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
    QObject::connect(pop, &LoadDialog::offerSelected, this, &MainWindow::loadOffer);
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

/* KLIENT */

void MainWindow::dodajKlient()
{
    NowyKlient* nowyKlient = new NowyKlient(this);
    nowyKlient->exec();
    delete nowyKlient;
}

void MainWindow::edytujKlient()
{
    EdycjaKlienta* okno = new EdycjaKlienta(this);
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
    QObject::connect(preview, SIGNAL(paintRequested(QPrinter *)), SLOT(print(QPrinter *))); //????
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
    const int d = (w-5)/2;                       //szerokość kolumny w szkielecie
    const int dw = 140;                          //szerokosc pierwszej kolumny w szkielecie poniżej tabeli

    *sDoc = "<html>\n"
            "<head>\n"
            "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>\n"
            "<title>Oferta</title>\n"
            "</head>\n"
            "<body>\n"
            "<table cellspacing=3>\n"
/*NAGŁÓWEK*/
            "<thead>\n"
            "<tr><td>\n"
            "\t<table>\n"
            "\t<tr>\n"
            "\t\t<td colspan=3><hr width=100%></td>\n"
            "\t</tr>\n"
            "\t<tr>\n"
            "\t\t<td valign=top width=";
    *sDoc += QString::number(d);
    *sDoc += ">\n"
             "\t\t\tNumer oferty: <font size=4>";
    *sDoc += *nr_oferty;
    *sDoc += "</font><br>\n"
             "\t\t\tData oferty: ";
    *sDoc += *data;
    *sDoc += "\n"
             "\t\t\t<hr width=100%>\n"
             "\t\t\tDla:<br>\n"
             "\t\t\t";
    *sDoc += klient->value("full").toString();
    *sDoc += "<br>\n"
             "\t\t\t";
    *sDoc += klient->value("adres").toString();
    *sDoc += "<br>\n"
             "\t\t\t";
    *sDoc += klient->value("tytul").toString();
    *sDoc += " ";
    *sDoc += klient->value("imie").toString();
    *sDoc += " ";
    *sDoc += klient->value("nazwisko").toString();
    *sDoc += "\n"
             "\t\t</td>\n";
/*linia pionowa*/
    *sDoc += "\t\t<td width=1 bgcolor=#000000>\n"
             "\t\t\t<br />\n"
             "\t\t</td>\n";
/*OD*/
    *sDoc += "\t\t<td width=";
    *sDoc += QString::number(d);
    *sDoc += ">\n"
             "\t\t\t";
    if(htm)
        *sDoc += "<img src=logo.png align=center><br>\n";
    else
        *sDoc += "<img src=:/logo align=center><br>\n";

    *sDoc += "\t\t\t<b>Marley Polska Sp. z o.o.</b><br>\n"
             "\t\t\tul. Dabrówki 6<br>\n"
             "\t\t\t03-909 Warszawa<br>\n"
             "\t\t\t<br>\n"
             "\t\t\t";
    /*adres bióra*/
    *sDoc += m_currentUser->adress().replace("\n", "\n\t\t\t");
    *sDoc += m_currentUser->mail();
    *sDoc += "\n"
             "\t\t</td>\n"
             "\t</tr>\n"
             "\t<tr>\n"
             "\t\t<td colspan=3><hr width=100%></td>\n"
             "\t</tr>\n"
             "\t</table>\n"
             "</td></tr>\n"
             "</thead>\n"
   /*Właściwa oferta*/
             "<tbody>\n"
             "<tr><td>\n"
             "\t";
    *sDoc += ui->plainTextEdit_zapytanie->toPlainText();
    *sDoc += "<br />\n"
             "</td></tr>\n"
             "<tr><td>\n";
 //tabela
    *sDoc += "\t<font face=\"Arial Narrow\" size=2>\n";   
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
