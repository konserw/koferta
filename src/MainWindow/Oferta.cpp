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

#include <QSqlQuery>
#include <QDate>

#include "LoadDialog.h"
#include "Wydruk.h"
#include "User.h"
#include "Database.h"
#include "Macros.h"

void MainWindow::nowyNumer()
{
    QDate d = QDate::currentDate();
    *data = d.toString("dd.MM.yyyy");
    ui->dateEdit->setDate(d);

    *nr_oferty = QString::number(u->nrOferty());
    nr_oferty->append("/");
    nr_oferty->append(d.toString("yyyy"));

    this->setTitle(nr_oferty);
}

void MainWindow::nowa()
{
    //czyszczenie starych danych
    ui->label->clear();
    id_klienta = -1;

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

    if(id_klienta == -1)
    {
        QMessageBox::warning(this, "brak danych", "Aby zapisanie oferty w bazie danych było możliwe należy wybrać klienta.");
        return;
    }

    int anr = nr_oferty->split('/').first().toInt();

    if(anr == u->nrOferty())
        u->nrOfertyInkrement();

    insert_zapisane(*nr_oferty, id_klienta, *data, u->uid(), ui->zapytanie->toPlainText(), ui->dostawa->toPlainText(), ui->termin->toPlainText(), ui->platnosc->toPlainText(), ui->oferta->toPlainText());

    for(int i=0; i < ui->tableWidget->rowCount() - 1; ++i)
        insert_zapisane_towary(*nr_oferty, ui->tableWidget->item(i, 0)->text(), ui->tableWidget->item(i, 5)->text(), ui->tableWidget->item(i, 3)->text());
}

void MainWindow::wczytaj()
{
    ww = new cLoadDialog(this);
    connect(ww, SIGNAL(sig(QString)), this, SLOT(wczytaj_oferte(QString)));
    ww->exec();
    delete ww;
}
void MainWindow::wczytaj_oferte(QString id)
{
    QString s;
    QSqlQuery q;

    s = "SELECT id_klienta, data, uid, zapytanie, dostawa, termin, platnosc, oferta FROM zapisane WHERE nr_oferty = '";
    s += id;
    s += "'";

    EXEC(s);

    q.next();

    this->init();

    *nr_oferty = id;
    this->setKlient(q.value(0).toInt());
    *data = q.value(1).toString();
    ui->zapytanie->setPlainText(q.value(3).toString());
    ui->dostawa->setPlainText(q.value(4).toString());
    ui->termin->setPlainText(q.value(5).toString());
    ui->platnosc->setPlainText(q.value(6).toString());
    ui->oferta->setPlainText(q.value(7).toString());

    s = "SELECT kod, ilosc, rabat FROM zapisane_towary WHERE nr_oferty = '";
    s += id;
    s += "'";
    EXEC(s);

    if(ui->tabWidget->isEnabled())
        this->clear();
    else
        this->init();

    int row;
    while(q.next())
    {
        s = q.value(0).toString();
        this->setTowar(s, q.value(1).toInt());
        row = ui->tableWidget->rowCount()-1;
        if(row < 0) row = 0;
        ui->tableWidget->item(row, 3)->setText(QString::number(q.value(2).toDouble()));
    }

    sum();

    this->setTitle(nr_oferty);
}


