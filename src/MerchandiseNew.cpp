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

#include <QtSql>
#include <QtDebug>
#include "MerchandiseNew.hpp"
#include "ui_MerchandiseNew.h"

MerchandiseNew::MerchandiseNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MerchandiseNew)
{
    ui->setupUi(this);
    ui->cena->setMaximum(99999);
    ui->r_szt->setChecked(true);
    ui->pushButton_ok->setDefault(true);

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(acc()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(close()));
}

void MerchandiseNew::acc()
{
    ui->pushButton_ok->setEnabled(false);

    /**********************
     * TODO
     * Przenieść do klasy Database !!!
     */

    QSqlQuery q;
    q.exec(QString("INSERT INTO `merchandise` (`id`, `code`, `description`, `price`, `unit`) VALUES (NULL, '%1', '%2', '%3', '%4')")
           .arg(ui->kod->text())
           .arg(ui->spec->text())
           .arg(ui->cena->value())
           .arg(ui->r_m->isChecked() ? "mb." : "szt.")
          );

    qDebug() << "Inserted merchandise  with id:" << ui->kod->text();

    this->accept();
}

MerchandiseNew::~MerchandiseNew()
{
    delete ui;
}
