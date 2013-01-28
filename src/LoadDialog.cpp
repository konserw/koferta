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
#include "../SzukajOferty.h"

cLoadDialog::cLoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog)
{
    ui->setupUi(this);
    of = new QString;

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->textEdit->setFont(font);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->widget, SIGNAL(selected(const QString&)), this, SLOT(ref(const QString&)));
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

void cLoadDialog::ref(const QString& id)
{
    QSqlQuery q;
    QString s, out;

    s = "SELECT DISTINCT zapisane.data, klient.short, klient.tytul, klient.nazwisko FROM klient, zapisane WHERE zapisane.nr_oferty = '";
    s += id;
    s += "' AND zapisane.id_klienta = klient.id";
    EXEC(s);
    q.next();

    out = "Numer oferty: ";
    out += id;
    out += "\nData oferty: ";
    out += q.value(0).toString();
    out += "\nKlient: ";
    out += q.value(1).toString();
    out += ", ";
    out += q.value(2).toString();
    out += " ";
  //  out += ", Pan(i) ";
    out += q.value(3).toString();
   // out += "\n\tTowary:\n
    out += "\nKod towatu:\t\tIlosc:\n";

    s = "SELECT kod, ilosc FROM zapisane_towary WHERE nr_oferty = '";
    s += id;
    s += "'";
    EXEC(s);

    QString kod;
    while(q.next())
    {
        kod = q.value(0).toString();
        out += kod;
        out += "\t\t";
        if(kod.size() < 10)
            out += "\t";
        if(kod.size() < 5)
            out += "\t";
        out += q.value(1).toString();
        out += "\n";
    }

    *of = id;
    ui->textEdit->setText(out);
}
