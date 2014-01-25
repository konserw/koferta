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
#include "Macros.h"
#include "OfferSearch.h"

#include <QSqlTableModel>

LoadDialog::LoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog)
{
    ui->setupUi(this);
    ui->label_towary->setText(tr("Towary:"));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->widget, &OfferSearch::selectionChanged, this, &LoadDialog::ref);

    model = new QSqlTableModel(this);
    model->setTable("savedOffersMerchandiseView");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    model->setHeaderData(4, Qt::Horizontal, tr("Kod"));
    model->setHeaderData(1, Qt::Horizontal, tr("Ilość"));
    model->setHeaderData(2, Qt::Horizontal, tr("Rabat"));

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->swapSections(0, 4);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(3);
    for(int i=5; i<8; ++i)
        ui->tableView->hideColumn(i);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

LoadDialog::~LoadDialog()
{
    delete ui;
}

void LoadDialog::ok()
{
    QString offerId = ui->widget->currentOffer();

    if(offerId.isNull())
    {
        this->reject();
        return;
    }

    emit offerSelected(offerId);
    this->accept();
}

void LoadDialog::ref(const QString& offerId)
{
    model->setFilter(QString("nr_oferty = '%1'").arg(offerId));
    model->select();
}
