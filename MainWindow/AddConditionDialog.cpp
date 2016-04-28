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

#include "AddConditionDialog.h"
#include "ui_AddConditionDialog.h"
#include "Database.h"
#include "TermItem.h"

AddConditionDialog::AddConditionDialog(TermItem::TermType type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddConditionDialog)
{
    ui->setupUi(this);

    m_type = type;
    QString typeTransaltion;
    switch(type)
    {
    case TermItem::termOffer:
        typeTransaltion = tr("\"Warunki Oferty\"");
        break;
    case TermItem::termPayment:
        typeTransaltion = tr("\"Warunki Płatności\"");
        break;
    case TermItem::termShipmentTime:
        typeTransaltion = tr("\"Termin Dostawy\"");
        break;
    case TermItem::termShipping:
        typeTransaltion = tr("\"Wraunki Dostawy\"");
        break;
    }

    this->setWindowTitle(tr("Nowa opcja w menu %1").arg(typeTransaltion));
    ui->label_typ->setText(tr("Dodaj nową opcję dla pola %1").arg(typeTransaltion));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
}

AddConditionDialog::~AddConditionDialog()
{
    delete ui;
}

void AddConditionDialog::ok()
{
    Database::createTerm(TermItem(m_type, ui->lineEdit_sort->text(), ui->textEdit->toPlainText()));
}
