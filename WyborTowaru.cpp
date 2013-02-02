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

#include "WyborTowaru.h"
#include "ui_WyborTowaru.h"
#include "delegate.h"

WyborTowaru::WyborTowaru(QWidget *parent) :
  QWidget(parent),
    ui(new Ui::WyborTowaru)
{
    ui->setupUi(this);

    ui->label->setText(tr("Filtr:"));
    ui->radioButton_id->setText(tr("Id produktu"));
    ui->radioButton_id->setChecked(true);
    ui->radioButton_name->setText(tr("Nazwa produktu"));
    ui->pushButton_close->setText(tr("Zamknij"));

    model = new QSqlTableModel(this);
    model->setTable("towar");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

  //  model->insertColumn(0, model);
    QStringList sl;
    sl << tr("Id") << tr("Nazwa") << tr("Cena Katalogowa") << tr("Sztuk");
    for(int i=0; i<sl.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, sl[i]);




    ui->tableView->setModel(model);
//    ui->tableView->hideColumn(4);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  //  ui->tableView->resizeColumnToContents(0);
   // ui->tableView->resizeColumnToContents(1);
    d = new SpinBoxDelegate;
    ui->tableView->setItemDelegateForColumn(3, d);
    //ui->tableView->openPersistentEditor();

    connect(d, SIGNAL(valueChanged(int,int)), this, SLOT(select(int,int)));
    connect(ui->radioButton_id, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(ui->radioButton_name, SIGNAL(clicked()), this, SLOT(ref2()));
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(ref(QString)));
    connect(ui->pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
}

WyborTowaru::~WyborTowaru()
{
    delete ui;
}

void WyborTowaru::select(int row, int value)
{
    QString id = model->record(row).value(1).toString();
    emit selectionChanged(id, value);
}

void WyborTowaru::ref2()
{
    this->ref(ui->lineEdit->text());
}

void WyborTowaru::ref(const QString & in)
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
