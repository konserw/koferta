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

TermsChooserDialog::TermsChooserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TermsChooserDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Terms Chooser");
    m_model = nullptr;

    connect(ui->listView, SIGNAL(activated(QModelIndex)), this, SLOT(selectionChanged(QModelIndex)));
}

TermsChooserDialog::TermsChooserDialog(QWidget *parent, TermModel *model, QString title) :
    QDialog(parent),
    ui(new Ui::TermsChooserDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);

    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectionChanged(QModelIndex)));

    setDataModel(model);
}

TermsChooserDialog::~TermsChooserDialog()
{
    delete ui;
    delete m_model;
}

TermItem TermsChooserDialog::choosenTerm()
{
    return *static_cast<TermItem*>(ui->listView->currentIndex().internalPointer());
}

void TermsChooserDialog::selectionChanged(QModelIndex idx)
{
    ui->plainTextEdit->setPlainText(idx.sibling(idx.row(), 2).data().toString());
}

void TermsChooserDialog::setDataModel(TermModel* model)
{
    m_model = model;
    ui->listView->setModel(m_model);
    ui->listView->setModelColumn(1);
}
