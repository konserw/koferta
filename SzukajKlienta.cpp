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
#include "SzukajKlienta.h"
#include "ui_SzukajKlienta.h"
#include <QSqlTableModel>
#include "Macros.h"

SzukajKlienta::SzukajKlienta(QWidget *parent) :
  QWidget(parent),
    ui(new Ui::SzukajKlienta)
{
    ui->setupUi(this);

    ui->radioButton_nazwa->setText(tr("Filtruj po nazwie"));
    ui->radioButton_nazwa->setChecked(true);
    ui->radioButton_nazwisko->setText(tr("Filtruj po nazwisku"));


    QStringList sl;
    sl << tr("Nr oferty") << tr("Klient") << tr("Data") << tr("Oferent");
    for(int i=0; i<sl.size(); ++i)
        ui->tabWidget->setTabText(i, sl[i]);

    model = new QSqlTableModel(this);
    model->setTable("klient");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    for(int i=0; i<sl.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, sl[i]);

    ui->tableView->setModel(model);
    for(int i=4; i<9; ++i)
        ui->tableView->hideColumn(i);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->resizeColumnToContents(3);

}

SzukajKlienta::~SzukajKlienta()
{
    delete ui;
}
