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

    
#include "SzukajTowaru.h"
#include "ui_SzukajTowaru.h"

SzukajTowaru::SzukajTowaru(QWidget *parent) :
  QWidget(parent),
    ui(new Ui::SzukajTowaru)
{
    ui->setupUi(this);

    ui->label->setText(tr("Filtr:"));
    ui->radioButton_id->setText(tr("Id produktu"));
    ui->radioButton_name->setText(tr("Nazwa produktu"));



}

SzukajTowaru::~SzukajTowaru()
{
    delete ui;
}

void SzukajTowaru::select(const QModelIndex &)
{
}

void SzukajTowaru::ref2()
{
}

void SzukajTowaru::ref(const QString &)
{
}
