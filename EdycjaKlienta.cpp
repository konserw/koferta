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

#include "EdycjaKlienta.h"
#include "ui_EdycjaKlienta.h"

#include <QSqlQuery>
#include <QString>

#include "Macros.h"

cEdycjaKlienta::~cEdycjaKlienta()
{
    delete ui;
}

cEdycjaKlienta::cEdycjaKlienta(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cEdycjaKlienta)
{
    DEBUG << "konstruktor";

    ui->setupUi(this);

    connect(ui->apply, SIGNAL(clicked()), this, SLOT(app()));
    connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->widget, SIGNAL(selectionChanged(int)), this, SLOT(change(int)));
    connect(ui->del, SIGNAL(clicked()), this, SLOT(del()));

    id = -1;
}

void cEdycjaKlienta::change(int _id)
{
    if(id == _id)
        return;

    id = _id;

    if(id == -1)
    {
        ui->skrocona->clear();
        ui->pelna->clear();
        ui->tytul->clear();
        ui->imie->clear();
        ui->nazwisko->clear();
        ui->adres->clear();
        return;
    }

    QString s;
    QSqlQuery q;

    DEBUG << "Zaznaczono klienta id " << id;

    s = "SELECT DISTINCT short, full, tytul, imie, nazwisko, adres FROM klient WHERE id=";
    s += QString::number(id);
    EXEC(s);
    q.next();

    ui->skrocona->setText(q.value(0).toString());
    ui->pelna->setText(q.value(1).toString());
    ui->tytul->setText(q.value(2).toString());
    ui->imie->setText(q.value(3).toString());
    ui->nazwisko->setText(q.value(4).toString());
    s = q.value(5).toString();
    s.remove("<br>");
    ui->adres->setPlainText(s);
}


void cEdycjaKlienta::app()
{
    if(id == 0) return;

    QString s, tmp;
    QSqlQuery q;

    s = "UPDATE klient SET short = '";
    s += ui->skrocona->text();
    s += "', full='";
    s += ui->pelna->text();
    s += "', tytul='";
    s += ui->tytul->text();
    s += "', imie='";
    s += ui->imie->text();
    s += "', nazwisko='";
    s += ui->nazwisko->text();
    s += "', adres='";
    tmp = ui->adres->toPlainText();
    tmp.replace("\n", "<br>\n");
    s += tmp;
    s += "' WHERE id=";
    s += QString::number(id);

    EXEC(s);
}

void cEdycjaKlienta::del()
{
    if(QMessageBox::warning(this, "Usuń klienta", "Czy na pewno chcesz usunąć tego klienta z bazy danych?", QMessageBox::Ok, QMessageBox::Cancel)==QMessageBox::Cancel)
        return;

    DEBUG <<  "usuwanie klienta id: " << id;
    QSqlQuery q;
    QString s;
    s = "DELETE FROM klient WHERE id=";
    s += QString::number(id);

    EXEC(s);

    change(-1);
}
