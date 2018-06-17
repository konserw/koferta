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

#include "CustomerEdit.hpp"
#include "ui_CustomerEdit.h"
#include "Database.hpp"
#include <QtDebug>

CustomerEdit::~CustomerEdit()
{
    delete ui;
}

CustomerEdit::CustomerEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomerEdit)
{
    qDebug() << "konstruktor";

    ui->setupUi(this);

    connect(ui->apply, &QPushButton::clicked, this, &CustomerEdit::app);
    connect(ui->close, &QPushButton::clicked, this, &CustomerEdit::close);
    connect(ui->widget, &CustomerSearch::selectionChanged, this, &CustomerEdit::change);
    connect(ui->del, &QPushButton::clicked, this, &CustomerEdit::del);
}

void CustomerEdit::change(Customer c)
{
    customer = c;

    ui->skrocona->setText(c.getShortName());
    ui->pelna->setText(c.getFullName());
    ui->tytul->setText(c.getTitle());
    ui->imie->setText(c.getName());
    ui->nazwisko->setText(c.getSurname());
    ui->adres->setPlainText(c.getAddress());
}


void CustomerEdit::app()
{
    if(customer.isValid() == false) return;

    customer.setShortName(ui->skrocona->text());
    customer.setFullName(ui->pelna->text());
    customer.setTitle(ui->tytul->text());
    customer.setName(ui->imie->text());
    customer.setSurname(ui->nazwisko->text());
    customer.setAddress(ui->adres->toPlainText());
    Database::instance()->editCustomer(customer);
}

void CustomerEdit::del()
{
    if(QMessageBox::warning(this, tr("Usuń klienta"), tr("Czy na pewno chcesz usunąć tego klienta z bazy danych?"), QMessageBox::Ok, QMessageBox::Cancel)==QMessageBox::Cancel)
        return;

    Database::instance()->deleteCustomer(customer);

    ui->skrocona->clear();
    ui->pelna->clear();
    ui->tytul->clear();
    ui->imie->clear();
    ui->nazwisko->clear();
    ui->adres->clear();
}
