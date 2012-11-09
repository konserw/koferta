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

#include "LoadDialog.h"
#include "ui_LoadDialog.h"

#include <QSqlQuery>
#include "Macros.h"

cLoadDialog::cLoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog)
{
    ui->setupUi(this);
    of = new QString;

    QTableWidgetItem* item;
    QSqlQuery q;
    QString s;
    int row = 0;

    s = "SELECT zapisane.nr_oferty, klient.short, zapisane.data FROM zapisane, klient WHERE zapisane.id_klienta = klient.id";

    EXEC(s);

    while(q.next())
    {
        ui->tableWidget->insertRow(row);
        for(int i=0; i<3; ++i)
        {
            item = new QTableWidgetItem(q.value(i).toString());
            ui->tableWidget->setItem(row, i, item);
        }
        row++;
    }

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(ref(int, int)));
}

cLoadDialog::~cLoadDialog()
{
    delete ui;
    delete of;
}

void cLoadDialog::ok()
{
    emit sig(*of);
    this->accept();
}

void cLoadDialog::ref(int w, int k)
{
    Q_UNUSED(k)
    QSqlQuery q;
    QString s, out;
    *of = ui->tableWidget->item(w, 0)->text();
    out = "Numer oferty: ";
    out += *of;
    out += "\nData oferty: ";
    out +=  ui->tableWidget->item(w, 2)->text();
    out += "\nKlient: ";
    out += ui->tableWidget->item(w, 1)->text();

    s = "SELECT DISTINCT klient.nazwisko FROM klient, zapisane WHERE zapisane.nr_oferty = '";
    s += *of;
    s += "' AND zapisane.id_klienta = klient.id";

    EXEC(s);

    q.next();

    out += ", ";
    out += q.value(0).toString();
    out += "\n\tTowary:\nKod towatu:\t\tIlosc:\n";

    s = "SELECT kod, ilosc FROM zapisane_towary WHERE nr_oferty = '";
    s += *of;
    s += "'";

    EXEC(s);

    while(q.next())
    {
        out += q.value(0).toString();
        out += "\t\t";
        out += q.value(1).toString();
        out += "\n";
    }

    ui->textEdit->setText(out);
}
