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

#include "TermsChooserDialog.h"
#include "ui_TermsChooserDialog.h"
#include "Database.h"
//#include <QAbstractTableModel>
#include "TermModel.h"
#include <QDebug>

TermsChooserDialog::TermsChooserDialog(QWidget *parent, TermItem::TermType type) :
    QDialog(parent),
    ui(new Ui::TermsChooserDialog)
{
    ui->setupUi(this);

    switch(type)
    {
    case TermItem::termOffer:
        setWindowTitle(tr("Wybór warunków oferty"));
        break;
    case TermItem::termPayment:
        setWindowTitle(tr("Wybór warunków płatności"));
        break;
    case TermItem::termShipmentTime:
        setWindowTitle(tr("Wybór terminu dostawy"));
        break;
    case TermItem::termShipping:
        setWindowTitle(tr("Wybór warunków dostawy"));
        break;
    default:
        qCritical() << "Invalid model selection";
    }

    ui->listView->setModel(Database::instance()->getTermModel(type));
    ui->listView->setModelColumn(1);

    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectionChanged(QModelIndex)));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TermsChooserDialog::ok);
}

TermsChooserDialog::~TermsChooserDialog()
{
    delete ui->listView->model();
    delete ui;
}

void TermsChooserDialog::selectionChanged(QModelIndex idx)
{
    ui->plainTextEdit->setPlainText(idx.sibling(idx.row(), 2).data().toString());
}

void TermsChooserDialog::ok()
{
    emit termChoosen(*static_cast<TermItem*>(ui->listView->currentIndex().internalPointer()));
}
