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

#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDate>
#include <QSqlQuery>
#include <QInputDialog>
#include <QList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QFileDialog>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QCalendarWidget>
#include <QPrintPreviewDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>

#include "SyntaxKlient.h"
#include "SyntaxTowar.h"
#include "NowyKlient.h"
#include "NowyTowar.h"
#include "Database.h"
#include "EdycjaTowaru.h"
#include "EdycjaKlienta.h"
#include "WyborTowaru.h"
#include "WyborKlienta.h"
#include "Logowanie.h"
#include "LoadDialog.h"
#include "User.h"
#include "Macros.h"

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

MainWindow::MainWindow ():
    QMainWindow(NULL),
    ui(new Ui::MainWindow)
{
    DEBUG << "konstruktor mainwindow";   
/**
  zmienne
 **/
    DEBUG << "inicjalizacja zmiennych";

    ui->setupUi(this);
    pln = false;

    //Stringi
    nr_oferty = new QString;
    data = new QString(QDate::currentDate().toString("dd.MM.yyyy"));

    //wyd = new cWydruk(ui, data, &id_klienta, nr_oferty);
    calendarWidget = new QCalendarWidget;
    klient = NULL;
/**
 connections
**/
    DEBUG << "połaczenia sygnałów i slotów";

    /*menu:*/
    //plik
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(nowa()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(popLoadDialog()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(zapisz()));
    connect(ui->actionNR, SIGNAL(triggered()), this, SLOT(nowyNumer()));
    connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
    //klient
    connect(ui->klientNowy, SIGNAL(triggered()), this, SLOT(dodajKlient()));
    connect(ui->klientImport, SIGNAL(triggered()), this, SLOT(importKlient()));
    connect(ui->klientEksport, SIGNAL(triggered()), this, SLOT(eksportKlient()));
    connect(ui->klientEdycja, SIGNAL(triggered()), this, SLOT(edytujKlient()));
    //towar
    connect(ui->towarNowy, SIGNAL(triggered()), this, SLOT(dodajTowar()));
    connect(ui->towarImport, SIGNAL(triggered()), this, SLOT(importTowar()));
    connect(ui->towarEksport, SIGNAL(triggered()), this, SLOT(eksportTowar()));
    connect(ui->towarEdycja, SIGNAL(triggered()), this, SLOT(edytujTowar()));
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

    //opcje wydruku
    connect(ui->pln, SIGNAL(pressed()), this, SLOT(pln_on()));
    connect(ui->eur, SIGNAL(pressed()), this, SLOT(pln_off()));
    connect(ui->kurs, SIGNAL(textChanged(QString)), this, SLOT(chKurs(QString)));

    //buttony w tabach
    connect(ui->addTowar, SIGNAL(clicked()), this, SLOT(popWyborTowaru()));
    connect(ui->rabat, SIGNAL(clicked()), this, SLOT(rabat()));
    connect(ui->delw, SIGNAL(clicked()), this, SLOT(del()));

    //Pozostałe informacje - odświerzanie zawartości pól tekstowych
    connect(ui->pushButton_wyborKlienta, SIGNAL(clicked()), this, SLOT(popWyborKlienta()));
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

    //inne
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(change(QTableWidgetItem*)));

/**
  ui
**/
    DEBUG << "user interface";

    ui->tabWidget->setCurrentIndex(0);

    //stan początkowy
    this->setTitle(NULL);
    ui->tab->setEnabled(false);
    ui->tab_2->setEnabled(false);

    ui->menuExport->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionNR->setEnabled(false);

/**
 Pozostałe informacje
**/

    ui->label_klient->setText(tr("Klient:"));
    ui->pushButton_wyborKlienta->setText(tr("Wybór klienta"));
    ui->plainTextEdit_klient->setReadOnly(true);

    ui->label_zapytanie->setText(tr("Zapytanie:"));
    ui->pushButton_zapytanieData->setText(tr("Kalendarz"));
    ui->checkBox_zapytanieData->setText(tr("Data zapytania:"));
    ui->checkBox_zapytanieData->setChecked(true);
    ui->checkBox_zapytanieNr->setText(tr("Numer zapytania:"));
    ui->checkBox_zapytanieNr->setChecked(false);
    ui->plainTextEdit_zapytanie->setReadOnly(true);

    dostawaModel = new QSqlTableModel;
    dostawaModel->setTable("dostawa");
    dostawaModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    dostawaModel->select();
    ui->comboBox_dostawa->setModel(dostawaModel);
    ui->comboBox_dostawa->setModelColumn(1);
    ui->plainTextEdit_dostawa->setReadOnly(true);
    ui->label_dostawa->setText(tr("Warunki dostawy:"));

    terminModel = new QSqlTableModel;
    terminModel->setTable("termin");
    terminModel->select();
    ui->comboBox_termin->setModel(terminModel);
    ui->comboBox_termin->setModelColumn(1);
    ui->plainTextEdit_termin->setReadOnly(true);
    ui->label_termin->setText(tr("Termin dostawy:"));

    platnoscModel = new QSqlTableModel;
    platnoscModel->setTable("platnosc");
    platnoscModel->select();
    ui->comboBox_platnosc->setModel(platnoscModel);
    ui->comboBox_platnosc->setModelColumn(1);
    ui->plainTextEdit_platnosc->setReadOnly(true);
    ui->label_platnosc->setText(tr("Warunki płatności:"));

    ofertaModel = new QSqlTableModel;
    ofertaModel->setTable("oferta");
    ofertaModel->select();
    ui->comboBox_oferta->setModel(ofertaModel);
    ui->comboBox_oferta->setModelColumn(1);
    ui->plainTextEdit_oferta->setReadOnly(true);
    ui->label_oferta->setText(tr("Warunki Oferty:"));

    ui->label_uwagi->setText(tr("Uwagi:"));
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

int MainWindow::ileTowaru(const QString& id)
{
    DEBUG << "search for existing idTowaru:" << id;
    QList<QTableWidgetItem*> list = ui->tableWidget->findItems(id, Qt::MatchFixedString);
    if(list.isEmpty())
    {
        DEBUG << "not found in table";
        return 0;
    }
    else
    {
        int row = list[0]->row();
        DEBUG << "found in row: " << row;
        return ui->tableWidget->item(row, 5)->text().toInt();
    }
}


void MainWindow::clear()
{
    while(ui->tableWidget->rowCount()){
        ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
    }

    ui->tableWidget->insertRow(0);
    QTableWidgetItem* item;
    item = new QTableWidgetItem("razem €");
    ui->tableWidget->setItem(0, 6, item);
    item = new QTableWidgetItem();
    ui->tableWidget->setItem(0, 7, item);
}
void MainWindow::rabat()
{
    uint rows = ui->tableWidget->rowCount();
    if(rows < 2)return;

    bool ok;
    double d = QInputDialog::getDouble(this, "Rabat", "Podaj domyślny rabat [%]:", 0, 0, 100, 2, &ok);
    if (ok)
       for(uint i=0; i<rows-1; i++){
        ui->tableWidget->item(i, 3)->setText(QString::number(d, 'f', 2));
           przelicz(i);
       }
    sum();
}

void MainWindow::sum()
{
    double sum=0;
    unsigned row = ui->tableWidget->rowCount()-1;
    for(unsigned i=0; i<row; ++i){
        sum += ev(i, 7);
    }
    ui->tableWidget->item(row, 7)->setText(QString::number(sum, 'f', 2));
}

void MainWindow::setTowar(const QSqlRecord& rec, int ile)
{

    QString id = rec.value("id").toString();
    QList<QTableWidgetItem*> list = ui->tableWidget->findItems(id, Qt::MatchFixedString);

    if(list.isEmpty())
    {
        QTableWidgetItem* item;

        int row = ui->tableWidget->rowCount()-1;
        if(row < 0) row = 0;

        ui->tableWidget->insertRow(row);

        //kod
        item = new QTableWidgetItem(id);
        ui->tableWidget->setItem(row, 0, item);
        //nazwa
        item = new QTableWidgetItem(rec.value("nazwa").toString());
        ui->tableWidget->setItem(row, 1, item);
        //cena kat
        double r = rec.value("cena").toDouble();
        item = new QTableWidgetItem(QString::number(r, 'f', 2));
        ui->tableWidget->setItem(row, 8, item);
        if(pln) r *= kurs;
        item = new QTableWidgetItem(QString::number(r, 'f', 2));
        ui->tableWidget->setItem(row, 2, item);
        //rabat
        item = new QTableWidgetItem("0");
        ui->tableWidget->setItem(row, 3, item);
        //cana
        item = new QTableWidgetItem("");
        ui->tableWidget->setItem(row, 4, item);
        //ilosc
        item = new QTableWidgetItem(QString::number(ile));
        ui->tableWidget->setItem(row, 5, item);
        //jednostka
        item = new QTableWidgetItem(rec.value("jednostka").toString());
        ui->tableWidget->setItem(row, 6, item);
        //koszt
        item = new QTableWidgetItem("");
        ui->tableWidget->setItem(row, 7, item);
        przelicz(row);
    }
    else
    {
        ui->tableWidget->item(list[0]->row(), 5)->setText(QString::number(ile));
    }

    sum();
}

double MainWindow::ev(unsigned row, unsigned col)
{
    return ui->tableWidget->item(row, col)->text().toDouble();
}
void MainWindow::przelicz(unsigned row)
{
    double r;
    r = 100 - ev(row, 3);
    r /= 100;
    r *= ev(row, 2);
    ui->tableWidget->item(row, 4)->setText(QString::number(r, 'f', 2));
    r = ui->tableWidget->item(row, 4)->text().toDouble();
    r *= ev(row, 5);
    ui->tableWidget->item(row, 7)->setText(QString::number(r, 'f', 2));
}

void MainWindow::change(QTableWidgetItem* item)
{
    if (!(item && item == ui->tableWidget->currentItem()))return;
    if(item->column() == 0 ||item->column() == 1)return;
    przelicz(item->row());
    sum();
}

void MainWindow::del()
{
    int row = ui->tableWidget->currentRow();
    if(row == ui->tableWidget->rowCount() -1)return; //wiersz z suma
    ui->tableWidget->removeRow(row);
    sum();
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
    connect(pop, SIGNAL(selectionChanged(QSqlRecord)), this, SLOT(clientChanged(QSqlRecord)));
    pop->exec();
    delete pop;
}

void MainWindow::popWyborTowaru()
{
    WyborTowaru* pop = new WyborTowaru(this);
    connect(pop, SIGNAL(countChanged(QSqlRecord,int)), this, SLOT(setTowar(QSqlRecord,int)));
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
    pln = true;

    ui->kurs->setEnabled(true);
    ui->kurs_label->setEnabled(true);
    ui->kurs->setText("4.00");

    ui->tableWidget->horizontalHeaderItem(2)->setText("Cena kat. zł");
    ui->tableWidget->horizontalHeaderItem(4)->setText("Cena zł");
    ui->tableWidget->horizontalHeaderItem(7)->setText("Wartość zł");

    tabupd();
}

void MainWindow::pln_off()
{
    kurs = 1;
    pln = false;
    ui->kurs->setEnabled(false);
    ui->kurs_label->setEnabled(false);

    ui->tableWidget->horizontalHeaderItem(2)->setText("Cena kat. €");
    ui->tableWidget->horizontalHeaderItem(4)->setText("Cena €");
    ui->tableWidget->horizontalHeaderItem(7)->setText("Wartość €");

    tabupd();
}

void MainWindow::chKurs(QString sKurs)
{
    bool ok;
    kurs = sKurs.toDouble(&ok);
    if(!ok)
    {
        kurs = 0;
    }
    tabupd();
}

void MainWindow::tabupd()
{
    double x;
    for(int i=0; i<ui->tableWidget->rowCount()-1; ++i)
    {
        if(pln)
        {
            x = ev(i, 8);
            x *= kurs;
            ui->tableWidget->item(i, 2)->setText(QString::number(x, 'f', 2));
        }
        else
        {
            ui->tableWidget->item(i, 2)->setText(ui->tableWidget->item(i, 8)->text());
        }
        przelicz(i);
    }
    sum();
}
/*************************
**      OFERTA          **
*************************/

void MainWindow::nowyNumer()
{
    QDate d = QDate::currentDate();
    *data = d.toString("dd.MM.yyyy");
    calendarWidget->setSelectedDate(d);

    *nr_oferty = QString::number(currentUser->nrOferty());
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
        this->clear();
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
    this->clear();
    for(int i=0; i<ui->tableWidget->columnCount(); i++)
        ui->tableWidget->setColumnWidth(i, 85);
    ui->tableWidget->setColumnWidth(1, 410);
    ui->tableWidget->setColumnWidth(0, 99);
    ui->tableWidget->setSortingEnabled(true);
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

    if(anr == currentUser->nrOferty())
        currentUser->nrOfertyInkrement();

    insert_zapisane(*nr_oferty, klient->value("id").toInt(), *data, currentUser->uid(), ui->lineEdit_zapytanieData->text(), ui->lineEdit_zapytanieNr->text(), ui->comboBox_dostawa->currentIndex(), ui->comboBox_termin->currentIndex(), ui->comboBox_platnosc->currentIndex(), ui->comboBox_oferta->currentIndex(), ui->plainTextEdit_uwagi->toPlainText());

    for(int i=0; i < ui->tableWidget->rowCount() - 1; ++i)
        insert_zapisane_towary(*nr_oferty, ui->tableWidget->item(i, 0)->text(), ui->tableWidget->item(i, 5)->text().toDouble(), ui->tableWidget->item(i, 3)->text().toDouble());
}

void MainWindow::popLoadDialog()
{
    LoadDialog* pop = new LoadDialog(this);
    connect(pop, SIGNAL(offerSelected(QSqlRecord, QSqlTableModel)), this, SLOT(loadOffer(QSqlRecord, QSqlTableModel)));
    pop->exec();
    delete pop;
}
void MainWindow::loadOffer(const QSqlRecord& rec, const QSqlTableModel& mod)
{
    *nr_oferty = rec.value("nr_oferty").toString();
    *data = rec.value("data").toString();

    this->init();
    this->setTitle(nr_oferty);

    QSqlQuery q;
    QString s;

    s = QString("SELECT DISTINCT id_klienta FROM zapisane WHERE nr_oferty = '%1'").arg(*nr_oferty);
    EXEC(s);
    q.next();

    QSqlQueryModel klientModel;
    s = QString("SELECT DISTINCT * FROM klient WHERE id = %1").arg(q.value(0).toInt());
    DEBUG << s;
    klientModel.setQuery(s);

    clientChanged(klientModel.record(0));

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

    QTableWidgetItem* item;
    double r;
    QSqlRecord record;
    for(int row=0; row<mod.rowCount(); ++row)
    {
        record = mod.record(row);

        s = QString("SELECT nazwa, cena, jednostka FROM towar WHERE id='%1'").arg(record.value("kod").toString());
        EXEC(s);
        q.next();

        ui->tableWidget->insertRow(row);

        //kod
        item = new QTableWidgetItem(record.value("kod").toString());
        ui->tableWidget->setItem(row, 0, item);
        //nazwa
        item = new QTableWidgetItem(q.value(0).toString());
        ui->tableWidget->setItem(row, 1, item);
        //cena kat
        r = q.value(1).toDouble();
        item = new QTableWidgetItem(QString::number(r, 'f', 2));
        ui->tableWidget->setItem(row, 8, item);
        if(pln) r *= kurs;
        item = new QTableWidgetItem(QString::number(r, 'f', 2));
        ui->tableWidget->setItem(row, 2, item);
        //rabat
        item = new QTableWidgetItem(record.value("rabat").toString());
        ui->tableWidget->setItem(row, 3, item);
        //cana
        item = new QTableWidgetItem("");
        ui->tableWidget->setItem(row, 4, item);
        //ilosc
        item = new QTableWidgetItem(record.value("ilosc").toString());
        ui->tableWidget->setItem(row, 5, item);
        //jednostka
        item = new QTableWidgetItem(q.value(2).toString());
        ui->tableWidget->setItem(row, 6, item);
        //koszt
        item = new QTableWidgetItem("");
        ui->tableWidget->setItem(row, 7, item);
        przelicz(row);
    }
    sum();
}

/*************************
**      BAZA DANYCH     **
*************************/

/* TOWAR */

void MainWindow::dodajTowar()
{
    NowyTowar* nowyTowar = new NowyTowar(this);
    nowyTowar->exec();
    delete nowyTowar;
}

void MainWindow::importTowar()
{
    QString s;
    s = QFileDialog::getOpenFileName(this, "Otwórz plik cennika", "", "Plik CSV (*.csv)");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć pliku z danymi cennika");
        if(!file.exists())DEBUG <<  "cennik nie iesnieje";
        else DEBUG <<  "cennik niedostępny";
        return;
    }

    QMessageBox* mb;
    mb = new QMessageBox(QMessageBox::Information, tr("Przetwarzanie bazy"), tr("Przetwarzanie bazy, proszę czekać..."), QMessageBox::NoButton, this);
    mb->setStandardButtons(0);
    mb->setModal(true);
    //mb->show();
    mb->exec();
    DEBUG <<  "wczytuje towar...";

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QStringList list;
    QSqlQuery q;
    QString sRead;
    QString cena;

    for(unsigned i=0; !in.atEnd(); ++i)
    {
        sRead = in.readLine();
        list = sRead.split("|");
        if(list.size() < 3)
        {
            DEBUG <<  "syntax error, wczyt towar, line: " << i;
            SyntaxTowar sw(this, sRead);
            sw.exec();
            continue;
        }

        if(list.size() > 3 && (list.at(3) == "mb." || list.at(3) == "m" || list.at(3) == "metr"))
            s = "mb.";
        else
            s = "szt.";

        cena =  list.at(2);
        cena.remove(' ');
        cena.replace(',', '.');

        insert_towar(list.at(1), list.at(0), cena.toDouble(), s);
    }
//    mb->hide();
    mb->accept();
    DEBUG <<  "koniec wczytywania";
}

void MainWindow::eksportTowar()
{
    QSqlQuery q;
    QString s;

    s = QFileDialog::getSaveFileName(this, "Zapisz plik cennika", "", "Plik CSV (*.csv)");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć plikudo zapisu");
        DEBUG <<  "Plik do zapisu towarów niedostępny";
        return;
    }

    QMessageBox* mb;
    mb = new QMessageBox(QMessageBox::Information, tr("Przetwarzanie bazy"), tr("Przetwarzanie bazy, proszę czekać..."), QMessageBox::NoButton, this);
    mb->setStandardButtons(0);
    mb->setModal(true);
    //mb->show();
    mb->exec();
    DEBUG <<  "zapis cennikow do pliku: " << s;

    QTextStream out(&file);
    out.setCodec("UTF-8");

    s = "SELECT id, nazwa, cena, jednostka FROM towar";

    EXEC(s);

    while(q.next())
    {
        out << q.value(1).toString() << "|" << q.value(0).toString() << "|" << q.value(2).toString() << "|" << q.value(3).toString() << "\n";
    }

    mb->accept();
    DEBUG <<  "koniec zapisu cennika";
}

void MainWindow::edytujTowar()
{
    EdycjaTowaru* okno = new EdycjaTowaru(this);
    okno->exec();
    delete okno;
}


/* KLIENT */

void MainWindow::dodajKlient()
{
    NowyKlient* nowyKlient = new NowyKlient(this);
    nowyKlient->exec();
    delete nowyKlient;
}

void MainWindow::importKlient()
{
    QString s;
    s = QFileDialog::getOpenFileName(this, "Otwórz plik bazy klientów", "", "Plik CSV (*.csv)");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć pliku z danymi klientów");
        if(!file.exists()) DEBUG <<  "klienci nie iesnieje";
        else DEBUG <<  "klienci niedostępny";
        return;
    }

    QMessageBox* mb;
    mb = new QMessageBox(QMessageBox::Information, tr("Przetwarzanie bazy"), tr("Przetwarzanie bazy, proszę czekać..."), QMessageBox::NoButton, this);
    mb->setStandardButtons(0);
    mb->setModal(true);
    //mb->show();
    mb->exec();
    DEBUG <<  "wczytuje klientow...";

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QStringList list;
    QString sRead;

    for(unsigned i=0; !in.atEnd(); ++i)
    {
        sRead = in.readLine();
        list = sRead.split("|");
        if(list.size() < 8)
        {
            DEBUG <<  "syntax error, wczyt_klient line: " << i;
            SyntaxKlient sw(this, sRead);
            sw.exec();
            continue;
        }

        s = list.at(5);
        s += "<br>\n";
        s += list.at(6);
        s += " ";
        s += list.at(7);
        insert_klient(list.at(4), list.at(3), list.at(0), list.at(1), list.at(2), s);
    }

    DEBUG <<  "koniec wczytywania";
    mb->accept();
}

void MainWindow::eksportKlient()
{
    QSqlQuery q;
    QString s;

    s = QFileDialog::getSaveFileName(this, "Zapisz plik bazy klientów", "", "Plik CSV (*.csv)");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć pliku do zapisu");
        return;
    }

    QMessageBox* mb;
    mb = new QMessageBox(QMessageBox::Information, tr("Przetwarzanie bazy"), tr("Przetwarzanie bazy, proszę czekać..."), QMessageBox::NoButton, this);
    mb->setStandardButtons(0);
    mb->setModal(true);
    //mb->show();
    mb->exec();
    DEBUG <<  "zapis klientow do pliku: " << s;

    QTextStream out(&file);
    out.setCodec("UTF-8");

    s = "SELECT nazwisko, imie, short, full, tytul, adres FROM klient";

    EXEC(s);

    while(q.next())
    {
        for(int i=0; i<7; i++)
            out << q.value(i).toString() << "|";
        out << "\n";
    }

    mb->accept();
    DEBUG <<  "koniec zapisu klientow";
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
    connect(preview, SIGNAL(paintRequested(QPrinter *)), SLOT(print(QPrinter *)));
    preview->exec();

    delete preview;
    delete printer;
}

void MainWindow::printPdf()
{
    htm = false;

    QString s;
    s = QFileDialog::getSaveFileName(this, "Zapis pdfa", "", "Dokument PDF (*.pdf)");
    if(s.isEmpty())return;

    QPrinter* printer = new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(s);
    print(printer);

    delete printer;
}

void MainWindow::printHtm()
{
    htm = true;

    QString s;
    s = QFileDialog::getSaveFileName(this, "Zapis do HTML", "", "Dokument HTML (*.htm)");
    if(s.isEmpty())return;

    QFile file(s);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        DEBUG << "plik " << s << " niedostępny";
        QMessageBox::warning(NULL, "error", "Nie udało się uzyskać dostępu do pliku");
        return;
    }

    QString* sDoc = new QString;
    makeDocument(sDoc);

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << *sDoc;

    delete sDoc;
}

void MainWindow::print(QPrinter *printer)
{
    const int margin = 5;                        //szerokość marginesu
    printer->setPaperSize(QPrinter::A4);
    printer->setResolution(300);
    printer->setPageMargins(margin, margin, margin, margin, QPrinter::Millimeter);

    QString* sDoc = new QString;
    makeDocument(sDoc);

    QFont* font = new QFont;
    font->setPointSize(10);
    font->setFamily("Arial");

    QTextDocument* doc = new QTextDocument;
    doc->setDefaultFont(*font);
    doc->setHtml(*sDoc);
    doc->print(printer);

    delete doc;
    delete font;
    delete sDoc;
}

void MainWindow::makeDocument(QString *sDoc)
{
    int kolumn = 3;
    QString waluta;
    uint rows = ui->tableWidget->rowCount()-1;

    const int w = 745;                           //szerokosc szkieletu dokumentu
    const int d = (w-5)/2;                       //szerokość kolumny w szkielecie
    const int dw = 140;                          //szerokosc pierwszej kolumny w szkielecie poniżej tabeli
    int z[8];                                    //szerokosc komorek tabeli
    z[0] = 20; //lp
    z[1] = 80; //kod
    //z2 - reszta szerokości
    z[3] = 60; //cena
    z[4] = 40; //rabat
    z[5] = 55; //cena2
    z[6] = 50; //ilosc+jedn
    z[7] = 60; //wartość

    z[2] = w - z[0] - z[7];
    if(ui->kol_kod->isChecked())
    {
        z[2] -= z[1];
        kolumn++;
    }
    if(ui->kol_cenaKat->isChecked())
    {
        z[2] -= z[3];
        kolumn++;
    }
    if(ui->kol_rabat->isChecked())
    {
        z[2] -= z[4];
        kolumn++;
    }
    if(ui->kol_cena->isChecked())
    {
        z[2] -= z[5];
        kolumn++;
    }
    if(ui->kol_ilosc->isChecked())
    {
        z[2] -= z[6];
        kolumn++;
    }
    z[2] -= kolumn*4;

    if(ui->eur->isChecked()) waluta= "€";
    else waluta = "zł";


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
        *sDoc += "<img src=logo.jpg align=center><br>\n";
    else
        *sDoc += "<img src=:/log align=center><br>\n";
    *sDoc +=
             "\t\t\t<b>Marley Polska Sp. z o.o.</b><br>\n"
             "\t\t\tul. Annopol 24<br>\n"
             "\t\t\t03-236 Warszawa<br>\n"
             "\t\t\t<br>\n"
             "\t\t\t";
    /*adres bióra*/
    *sDoc += currentUser->adress().replace("\n", "\n\t\t\t");
    *sDoc += currentUser->mail();
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
    *sDoc += "\t<font face=\"Arial Narrow\" size=2>\n"
             "\t<table cellspacing=3>\n"
             "\t<thead><tr>\n"
             "\t\t<td width=";
    *sDoc += QString::number(z[0]);
    *sDoc += ">LP</td>\n";
    if(ui->kol_kod->isChecked())
    {
        *sDoc += "\t\t<td width=";
        *sDoc += QString::number(z[1]);
        *sDoc += ">Kod</td>\n";
    }
    if(ui->kol_towar->isChecked())
    {
        *sDoc += "\t\t<td width=";
        *sDoc += QString::number(z[2]);
        *sDoc += ">Specyfikacja</td>\n";
    }
    if(ui->kol_ilosc->isChecked())
    {
        *sDoc += "\t\t<td width=";
        *sDoc += QString::number(z[6]);
        *sDoc += " align = right>Ilość</td>\n";
    }
    if(ui->kol_cenaKat->isChecked())
    {
        *sDoc += "\t\t<td width=";
        *sDoc += QString::number(z[3]);
        *sDoc += " align = right>Cena kat. ";
        *sDoc += waluta;
        *sDoc += "</td>\n";
    }
    if(ui->kol_rabat->isChecked())
    {
        *sDoc += "\t\t<td width=";
        *sDoc += QString::number(z[4]);
        *sDoc += " align = right>Rabat</td>\n";
    }
    if(ui->kol_cena->isChecked())
    {
        *sDoc += "\t\t<td width=";
        *sDoc += QString::number(z[5]);
        *sDoc += " align = right>Cena ";
        *sDoc += waluta;
        *sDoc += "</td>\n";
    }
    *sDoc += "\t\t<td width=";
    *sDoc += QString::number(z[7]);
    *sDoc += " align = right>Wartość ";
    *sDoc += waluta;
    *sDoc += "</td>\n"
          "\t</tr></thead>\n";

    for(uint i=0; i<rows; ++i){
        *sDoc += "\t<tr>\n\t\t<td>";
        *sDoc += QString::number(i+1);
        *sDoc += "</td>\n";
        if(ui->kol_kod->isChecked())
        {
            *sDoc += "\t\t<td>";
            *sDoc += ui->tableWidget->item(i, 0)->text();
            *sDoc += "</td>\n";
        }
        if(ui->kol_towar->isChecked())
        {
            *sDoc += "\t\t<td>";
            *sDoc += ui->tableWidget->item(i, 1)->text();
            *sDoc += "</td>\n";
        }
        if(ui->kol_ilosc->isChecked())
        {
            *sDoc += "\t\t<td align=right>";
            *sDoc += ui->tableWidget->item(i, 5)->text();
            *sDoc += " ";
            *sDoc += ui->tableWidget->item(i, 6)->text();
            *sDoc += "</td>\n";
        }
        if(ui->kol_cenaKat->isChecked())
        {
            *sDoc += "\t\t<td align = right>";
            *sDoc += ui->tableWidget->item(i, 2)->text();
            *sDoc += "</td>\n";
        }
        if(ui->kol_rabat->isChecked())
        {
            *sDoc += "\t\t<td align = right>";
            *sDoc += ui->tableWidget->item(i, 3)->text();
            *sDoc += "%</td>\n";
        }
        if(ui->kol_cena->isChecked())
        {
            *sDoc += "\t\t<td align = right>";
            *sDoc += ui->tableWidget->item(i, 4)->text();
            *sDoc += "</td>\n";
        }
        *sDoc += "\t\t<td align = right>";
        *sDoc += ui->tableWidget->item(i, 7)->text();
        *sDoc += "</td>\n"
                 "\t</tr>\n";
    }
    *sDoc += "\t<tr>\n"
            "\t\t<td colspan=";
    *sDoc += QString::number(kolumn-1);
    *sDoc += " align=right>Razem ";
    *sDoc += waluta;
    *sDoc += ":</td>\n"
            "\t\t<td align=right>";
    *sDoc += ui->tableWidget->item(rows, 7)->text();
    *sDoc += "</td>\n"
            "\t</tr>\n"
            "\t</table></font>\n"
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
   *sDoc += ui->plainTextEdit_uwagi->toPlainText();
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
    if(!currentUser->male()) *sDoc += "a";
    *sDoc += "<br>\n"
            "\t\t";
    *sDoc += currentUser->name();
    *sDoc += "\n"
            "\t</p>\n"
            "</td></tr>\n"
            "</tbody>\n"
            "</table>\n"
            "</body>\n"
            "</html>\n";
}
