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

#include "CustomerEdit.h"
#include "ui_CustomerEdit.h"
#include "Database.h"
#include <QSqlQuery>
#include <QString>
#include <QtSql>

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

    connect(ui->apply, SIGNAL(clicked()), this, SLOT(app()));
    connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->widget, SIGNAL(selectionChanged(QSqlRecord)), this, SLOT(change(QSqlRecord)));
    connect(ui->del, SIGNAL(clicked()), this, SLOT(del()));
}

void CustomerEdit::change(const QSqlRecord& rec)
{
    QString s;
    id = rec.value("id").toString();

    ui->skrocona->setText(rec.value("short").toString());
    ui->pelna->setText(rec.value("full").toString());
    ui->tytul->setText(rec.value("tytul").toString());
    ui->imie->setText(rec.value("imie").toString());
    ui->nazwisko->setText(rec.value("nazwisko").toString());
    s = rec.value("adres").toString();
    s.remove("<br>");
    ui->adres->setPlainText(s);
}


void CustomerEdit::app()
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
    s += id;

    EXEC_SILENT(s);
}

void CustomerEdit::del()
{
    if(QMessageBox::warning(this, "Usuń klienta", "Czy na pewno chcesz usunąć tego klienta z bazy danych?", QMessageBox::Ok, QMessageBox::Cancel)==QMessageBox::Cancel)
        return;

    qDebug() <<  "usuwanie klienta id: " << id;
    QSqlQuery q;
    QString s;
    s = "DELETE FROM klient WHERE id=";
    s += id;

    EXEC_SILENT(s);

    ui->skrocona->clear();
    ui->pelna->clear();
    ui->tytul->clear();
    ui->imie->clear();
    ui->nazwisko->clear();
    ui->adres->clear();
}
