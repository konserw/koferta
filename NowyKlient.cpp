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

#include "NowyKlient.h"
#include "ui_NowyKlient.h"
#include "Database.h"


NowyKlient::NowyKlient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NowyKlient)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(acc()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

NowyKlient::~NowyKlient()
{
    delete ui;
}

void NowyKlient::acc(){
    QString adres = ui->adres->toPlainText();
    adres.replace("\n", "<br>\n");
    insert_klient(ui->skrocona->text(), ui->pelna->text(), ui->tytul->text(), ui->imie->text(), ui->nazwisko->text(), adres);
    this->accept();
}



