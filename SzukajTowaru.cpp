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

#include <QSqlTableModel>
#include "Macros.h"
#include <QSqlRecord>

#include "SzukajTowaru.h"
#include "ui_SzukajTowaru.h"
#include "delegate.h"

SzukajTowaru::SzukajTowaru(QWidget *parent) :
  QWidget(parent),
    ui(new Ui::SzukajTowaru)
{
    ui->setupUi(this);

    ui->label->setText(tr("Filtr:"));
    ui->radioButton_id->setText(tr("Id produktu"));
    ui->radioButton_id->setChecked(true);
    ui->radioButton_name->setText(tr("Nazwa produktu"));

    model = new QSqlTableModel(this);
    model->setTable("towar");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    QStringList sl;
    sl << tr("Id") << tr("Nazwa") << tr("Cena Katalogowa");
    for(int i=0; i<sl.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, sl[i]);

    ui->tableView->setModel(model);
    ui->tableView->hideColumn(3);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  //  ui->tableView->resizeColumnToContents(0);
   // ui->tableView->resizeColumnToContents(1);

    connect(ui->radioButton_id, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(ui->radioButton_name, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(ref(QString)));
}

SzukajTowaru::~SzukajTowaru()
{
    delete ui;
}

void SzukajTowaru::select(int row, int value)
{
    QString id = model->record(row).value(1).toString();
    emit selectionChanged(id, value);
}

void SzukajTowaru::ref2()
{
    this->ref(ui->lineEdit->text());
}

void SzukajTowaru::ref(const QString & in)
{
    QString s;
    if(ui->radioButton_id->isChecked())
        s = "id like '";
    else
        s = "nazwa like '";
    s += in;
    s += "%'";
    model->setFilter(s);
}
