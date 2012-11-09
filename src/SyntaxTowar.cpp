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

#include "SyntaxTowar.h"
#include "ui_SyntaxTowar.h"
#include "Database.h"

syntax_towar::syntax_towar(QWidget *parent, QString& str) :
    QDialog(parent),
    ui(new Ui::syntax_towar)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(acc()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    ui->textBrowser->setText(str);
}

syntax_towar::~syntax_towar()
{
    delete ui;
}
void syntax_towar::acc()
{
    QString s;
    if(ui->r_m->isChecked())
        s = "metr";
    else
        s = "szt.";

    insert_towar(ui->kod->text(), ui->spec->text(), ui->cena->text(), s);
    this->accept();
}
