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

AddConditionDialog::AddConditionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddConditionDialog)
{
    ui->setupUi(this);
    ui->comboBox->addItems(QStringList()
                           << tr("Wraunki Dostawy")
                           << tr("Warunki Oferty")
                           << tr("Warunki Płatności")
                           << tr("Termin Dostawy")
                           );
    disconnect(ui->buttonBox, SIGNAL(accepted()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
}

AddConditionDialog::~AddConditionDialog()
{
    delete ui;
}

void AddConditionDialog::ok()
{
    if(Database::createTerm(TermItem(
                                static_cast<TermItem::TermType>(ui->comboBox->currentIndex()),
                                ui->lineEdit_sort->text(),
                                ui->textEdit->toPlainText()
                                )) == false)
    {
        QMessageBox::critical(
                    this,
                    tr("Błąd bazy danych"),
                    tr("Wystąpił błąd podczas zapisywania danych do bazy!")
                    );
    }//TODO: nie zamykac dialogu? -> zrobione?
}
