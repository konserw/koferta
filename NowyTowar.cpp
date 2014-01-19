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

#include "NowyTowar.h"
#include "ui_NowyTowar.h"
#include "Database.h"
#include <QtSql>
#include <QMessageBox>
#include "Macros.h"

NowyTowar::NowyTowar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NowyTowar)
{
    ui->setupUi(this);
    ui->cena->setMaximum(99999);
    ui->r_szt->setChecked(true);
    ui->pushButton_cancel->setText(tr("Anuluj"));
    ui->pushButton_ok->setText(tr("OK"));
    ui->pushButton_ok->setDefault(true);

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(acc()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(close()));
}

void NowyTowar::acc()
{
    ui->pushButton_ok->setEnabled(false);

    QSqlQuery q;
    QString kod = ui->kod->text();
    QString s = QString("Select * FROM towar WHERE id='%1'").arg(kod);

    EXEC(s);
    if(q.next())
    {
        QMessageBox::warning(this, tr("Duplikat kodu"), tr("Kod towaru musi być unikalny. Spróbuj wprowadzić inny kod."));
        ui->pushButton_ok->setEnabled(true);
        return;
    }

    if(ui->r_m->isChecked())
        s = "mb.";
    else
        s = "szt.";
    insert_towar(kod, ui->spec->text(), ui->cena->value(), s);

    this->accept();
}

NowyTowar::~NowyTowar()
{
    delete ui;
}
