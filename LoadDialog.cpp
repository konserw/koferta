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
#include <QProcess>
#include "LoadDialog.h"
#include "ui_LoadDialog.h"
#include "OfferSearch.h"
#include "LoadDialogMerchandiseListModel.h"
#include "Database.h"

LoadDialog::LoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog)
{
    ui->setupUi(this);
    ui->label_towary->setText(tr("Towary:"));

    model = Database::loadDialogMerchandiseListModel(this);

    ui->tableView->setModel(model);
    ui->tableView->hideColumn(4);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->widget, &OfferSearch::selectionChanged, model, &LoadDialogMerchandiseListModel::setOfferId);
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
