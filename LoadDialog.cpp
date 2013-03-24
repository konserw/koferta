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
#include "SzukajOferty.h"
#include <QSqlRecord>
#include <QSqlTableModel>

LoadDialog::LoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog)
{
    ui->setupUi(this);
    ui->label_towary->setText(tr("Towary:"));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->widget, SIGNAL(selectionChanged(const QSqlRecord&)), this, SLOT(ref(const QSqlRecord&)));

    model = new QSqlTableModel(this);
    model->setTable("zapisane_towary");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    QStringList sl;
    sl << tr("Kod") << tr("Ilość") << tr("Rabat");
    for(int i=1; i<4; ++i)
        model->setHeaderData(i, Qt::Horizontal, sl[i-1]);

    ui->tableView->setModel(model);
    ui->tableView->hideColumn(0);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    cur = NULL;
}

LoadDialog::~LoadDialog()
{
    delete ui;
    delete cur;
}

void LoadDialog::ok()
{
    emit offerSelected(*cur, *model);
    this->accept();
}

void LoadDialog::ref(const QSqlRecord& rec)
{
    delete cur;
    cur = new QSqlRecord(rec);

    model->setFilter(QString("nr_oferty = '%1'").arg(rec.value("nr_oferty").toString()));
    model->select();
}
