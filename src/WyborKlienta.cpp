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

#include "WyborKlienta.h"
#include "ui_WyborKlienta.h"

#include <QSqlQuery>
#include "Macros.h"


cWyborKlienta::cWyborKlienta(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WyborKlienta)
{
    ui->setupUi(this);

    ui->tableWidget->hideColumn(0);

    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->ok, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(ref(QString)));

    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 150);
}

cWyborKlienta::~cWyborKlienta()
{
    delete ui;
}

void cWyborKlienta::ok()
{
    ui->lineEdit->clear();
    QList<QTableWidgetItem*> l = ui->tableWidget->selectedItems();
    if (!l.isEmpty())
        emit id_klienta(ui->tableWidget->item(l.at(0)->row(), 0)->text());
    this->close();
}
void cWyborKlienta::ref(QString in)
{
    if(ui->lineEdit->text() != in) ui->lineEdit->setText(in);
    while(ui->tableWidget->rowCount()){
        ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
    }
    if(in.size()==0)return;

    QString s;
    QSqlQuery q;
    QTableWidgetItem* item;
    int row = 0;

    s = "SELECT id, short, nazwisko FROM klient WHERE short LIKE '";
    s += in;
    s += "%';";

    EXEC(s);

    while(q.next()){
        ui->tableWidget->insertRow(row);
        for(int i=0; i<3; i++){
            item = new QTableWidgetItem(q.value(i).toString());
            ui->tableWidget->setItem(row, i, item);
        }
        row++;
    }
}
