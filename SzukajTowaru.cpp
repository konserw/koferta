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
    sl << tr("Id") << tr("Nazwa");// << tr("Cena Katalogowa");
    //model->setHorizontalHeaderLabels(sl);
    for(int i=0; i<sl.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, sl[i]);

    ui->tableView->setModel(model);
    ui->tableView->setSortingEnabled(true);


    ui->tableView->resizeColumnToContents(0);
 //   ui->tableView->resizeColumnToContents(1);
    ui->tableView->hideColumn(2);
    ui->tableView->hideColumn(3);

    QHeaderView* hdr = ui->tableView->horizontalHeader();
 //   hdr->setResizeMode(0, QHeaderView::ResizeToContents);
    hdr->setResizeMode(1, QHeaderView::Stretch);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->radioButton_id, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(ui->radioButton_name, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(ref(QString)));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(select(QModelIndex)));

    QItemSelectionModel *sm = ui->tableView->selectionModel();
    connect(sm, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(currentRowChanged(QModelIndex,QModelIndex)));
}

SzukajTowaru::~SzukajTowaru()
{
    delete ui;
}

void SzukajTowaru::select(const QModelIndex& idx)
{
    QSqlRecord r = model->record(idx.row());
    if(!r.isEmpty())
        emit selectionChanged(r);
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

void SzukajTowaru::currentRowChanged(const QModelIndex &cur, const QModelIndex &prev)
{
    Q_UNUSED(prev);
    select(cur);
}
