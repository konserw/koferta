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

#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDate>
#include <QSqlQuery>
#include <QInputDialog>

#include "WyborTowaru.h"
#include "WyborKlienta.h"
#include "Logowanie.h"
#include "LoadDialog.h"
#include "Wydruk.h"
#include "User.h"
#include "Macros.h"

MainWindow::~MainWindow()
{
    DEBUG << "destruktor main window";

    delete ui;
    delete id_klienta;
    delete nr_oferty;
    delete data;
    delete dostawa;
    delete platnosc;
    delete termin;
    delete oferta;
    delete u;
    delete wyd;


    DEBUG << "aplikacja zkończona prawidłowo";
}

MainWindow::MainWindow(cUser* us) :
    QMainWindow(NULL),
    ui(new Ui::MainWindow)
{
    DEBUG << "konstruktor mainwindow";

    ui->setupUi(this);

    //stan początkowy
    this->setTitle(NULL);
    ui->tab->setEnabled(false);
    ui->tab_2->setEnabled(false);

    ui->menuExport->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionNR->setEnabled(false);

    pln = false;

    //okna
    tw = new cWyborTowaru(this);
    kw = new cWyborKlienta(this);
    mb = new QMessageBox(QMessageBox::Information, "Przetwarzanie bazy", "Przetwarzanie bazy, proszę czekać...", QMessageBox::NoButton, this);
    mb->setStandardButtons(0);
    mb->setModal(true);
    //Stringi
    nr_oferty = new QString;
    id_klienta = new QString;
    data = new QString(QDate::currentDate().toString("dd.MM.yyyy"));
    //inne
    u = us;
    wyd = new cWydruk(ui, data, id_klienta, nr_oferty);

/**
 connections
**/
    /*menu:*/
    //plik
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(nowa()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(wczytaj()));
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
    connect(ui->actionDo_HTML, SIGNAL(triggered()), wyd, SLOT(do_htm()));
    connect(ui->actionDo_PDF, SIGNAL(triggered()), wyd, SLOT(do_pdf()));
    connect(ui->actionDruk, SIGNAL(triggered()), wyd, SLOT(prev()));
    //info:
    connect(ui->actionO_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionO_kOferta, SIGNAL(triggered()), this, SLOT(about()));


    //automatyczne zapisywanie
    connect(wyd, SIGNAL(zapisz()), this, SLOT(zapisz()));

    //opcje wydruku
    connect(ui->pln, SIGNAL(pressed()), this, SLOT(pln_on()));
    connect(ui->eur, SIGNAL(pressed()), this, SLOT(pln_off()));
    connect(ui->kurs, SIGNAL(textChanged(QString)), this, SLOT(chKurs(QString)));

    //buttony w tabach
    connect(ui->addTowar, SIGNAL(clicked()), tw, SLOT(exec()));
    connect(ui->wyborKlienta, SIGNAL(clicked()), kw, SLOT(exec()));
    connect(ui->rabat, SIGNAL(clicked()), this, SLOT(rabat()));
    connect(ui->delw, SIGNAL(clicked()), this, SLOT(del()));

    //combosy - odświerzanie zawartości pól tekstowych
    connect(ui->dostawaCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(ref(QString)));
    connect(ui->platnosCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(ref2(QString)));
    connect(ui->terminCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(ref3(QString)));
    connect(ui->ofertaCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(ref4(QString)));

    //inne
    connect(kw, SIGNAL(id_klienta(QString)), this, SLOT(setKlient(QString)));
    connect(tw, SIGNAL(sig(QHash<QString, int>)), this, SLOT(setTowar(QHash<QString, int>)));
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(change(QTableWidgetItem*)));
/**
 combosy
**/
    dostawa = new QHash<QString, QString>;
    dostawa->insert("dostawcy, pow 3000", "Na koszt dostawcy przy zamówieniu powyżej 3000,- zł. netto. (nie dot. rur w odcinkach 5 m)");
    dostawa->insert("dostawcy", "Na koszt dostawcy.");
    dostawa->insert("odbiorcy", "Na koszt odbiorcy z magazynu w Warszawie.");
    ui->dostawaCombo->addItems(dostawa->keys());

    platnosc = new QHash<QString, QString>;
    platnosc->insert("przelew 14", "Przelewem w terminie 14 dni od daty wystawienia faktury w złotych polskich wg. kursu sprzedaży euro w NBP obowiązującego w dniu wystawienia faktury.");
    platnosc->insert("przelew 30", "Przelewem w terminie 30 dni od daty wystawienia faktury w złotych polskich wg. kursu sprzedaży euro w NBP obowiązującego w dniu wystawienia faktury.");
    platnosc->insert("przedpłata", "Przedpłata w złotych polskich wg. kursu sprzedaży euro w NBP obowiązującego w dniu wystawienia faktury.");
    platnosc->insert("pobranie", "Za pobraniem w złotych polskich wg. kursu sprzedaży euro w NBP obowiązującego w dniu wystawienia faktury.");
    platnosc->insert("przelew 14 zl", "Przelewem w terminie 14 dni od daty wystawienia faktury.");
    platnosc->insert("przelew 30 zl", "Przelewem w terminie 30 dni od daty wystawienia faktury.");
    platnosc->insert("przedpłata zl", "Przedpłata.");
    platnosc->insert("pobranie zl", "Za pobraniem.");
    ui->platnosCombo->addItems(platnosc->keys());

    termin = new QHash<QString, QString>;
    termin->insert("natychmiastowy", "1-2 dni roboczych od daty złożenia zamówienia");
    termin->insert("2-3 tyg", "2 do 3 tygodni roboczych od daty złożenia zamówienia");
    termin->insert("4-6 tyg", "4 do 6 tygodni roboczych od daty złożenia zamówienia");
    ui->terminCombo->addItems(termin->keys());

    oferta = new QHash<QString, QString>;
    oferta->insert("3 miesiące", "Niniejsza oferta jest ważna bez zobowiązań w okresie trzech miesięcy od daty jej sporządzenia.");
    oferta->insert("1 miesiąc", "Niniejsza oferta jest ważna bez zobowiązań w okresie jednego miesięca od daty jej sporządzenia.");
    ui->ofertaCombo->addItems(oferta->keys());

    connect(ui->calendarWidget, SIGNAL(selectionChanged()), this, SLOT(calChanged()));
    connect(ui->dateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(dateChanged(QDate)));
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
    QString s = "kOferta v.";
    s += QString::number(VER);

    if(nr != NULL)
    {
        s += " | oferta nr: ";
        s += *nr;
    }

    this->setWindowTitle(s);
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

void MainWindow::setTowar(QHash<QString, int> hasz)
{
    QSqlQuery q;
    QString s;
    QHash<QString, int>::iterator it;
    int row = ui->tableWidget->rowCount()-1;
    if(row<0)row=0;
    bool ok;

    for(it = hasz.begin(); it != hasz.end(); ++it)
    {
        s = "SELECT nazwa, cena_kat, jednostka FROM towar WHERE id='";
        s += it.key();
        s+= "'";

        EXEC(s);

        q.next();

        ui->tableWidget->insertRow(row);
        QTableWidgetItem* item;
        //kod
        item = new QTableWidgetItem(it.key());
        ui->tableWidget->setItem(row, 0, item);
        //nazwa
        item = new QTableWidgetItem(q.value(0).toString());
        ui->tableWidget->setItem(row, 1, item);
        //cena kat
        double r ;//= q.value(1).toDouble(&ok);
 //       if(!ok)
 //       {

            QString x = q.value(1).toString();
  //          DEBUG << x;
            x.replace(",", ".");
            DEBUG << x;
            r = x.toDouble(&ok);
            if(!ok)
            {
                DEBUG << x;
                x.replace(".", ",");
                DEBUG << x;
                r = x.toDouble(&ok);
            }
 //       }
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
        item = new QTableWidgetItem(QString::number(it.value()));
        ui->tableWidget->setItem(row, 5, item);
        //jednostka
        item = new QTableWidgetItem(q.value(2).toString());
        ui->tableWidget->setItem(row, 6, item);
        //koszt
        item = new QTableWidgetItem("");
        ui->tableWidget->setItem(row, 7, item);
        przelicz(row);

        row++;
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

void MainWindow::ref(QString id)
{
    ui->dostawa->setPlainText(dostawa->value(id));
}
void MainWindow::ref2(QString id)
{
    ui->platnosc->setPlainText(platnosc->value(id));
}
void MainWindow::ref3(QString id)
{
    ui->termin->setPlainText(termin->value(id));
}
void MainWindow::ref4(QString id)
{
    ui->oferta->setPlainText(oferta->value(id));
}

void MainWindow::calChanged()
{
    QDate d;
    d = ui->calendarWidget->selectedDate();
    ui->dateEdit->setDate(d);
}

void MainWindow::dateChanged(QDate data)
{
    ui->calendarWidget->setSelectedDate(data);
    QString s;
    s = "W odpowiedzi na zapytanie z dnia ";
    s += data.toString("dd.MM.yyyy");
    s += " przedstawiamy ofertę na dostawę następujących produktów:";
    ui->zapytanie->setPlainText(s);
}

void MainWindow::setKlient(QString id)
{
    QString s;
    QSqlQuery q;

    *id_klienta = id;

    s = "SELECT DISTINCT short, nazwisko FROM klient WHERE id='";
    s += id;
    s += "'";

    EXEC(s);

    q.next();
    s = q.value(0).toString();
    s += " | ";
    s += q.value(1).toString();
    ui->label->setText(s);
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
