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

#include "EdycjaTowaru.h"
#include "ui_EdycjaTowaru.h"

#include <QSqlQuery>


#include "Macros.h"


EdycjaTowaru::EdycjaTowaru(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EdycjaTowaru)
{
    DEBUG << "Konstruktor cEdycjaTowaru";

    ui->setupUi(this);

    connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(app()));
    connect(ui->widget, SIGNAL(selectionChanged(const QSqlRecord&)), this, SLOT(click(const QSqlRecord&)));
    connect(ui->del, SIGNAL(clicked()), this, SLOT(del()));
}

EdycjaTowaru::~EdycjaTowaru()
{
    delete ui;
}

void EdycjaTowaru::del()
{
    QString s;
    QSqlQuery q;

    if(QMessageBox::warning(this, "Usuń towar", "Czy na pewno chcesz usunąć ten towar z bazy danych?", QMessageBox::Ok, QMessageBox::Cancel)==QMessageBox::Cancel)
        return;

    DEBUG << "usuwanie towaru, kod: " << ui->kod->text();

    s = "DELETE FROM towar WHERE kod='";
    s += ui->kod->text();
    s += "'";
    EXEC(s);

    this->click();
}

void EdycjaTowaru::click(const QSqlRecord& rec)
{
    if(rec.isEmpty())
    {
        ui->kod->clear();
        ui->spec->clear();
        ui->cena->clear();
        return;
    }


    ui->kod->setText(rec.value(0).toString());
    ui->spec->setText(rec.value(1).toString());
    ui->cena->setText(rec.value(2).toString());
    if(rec.value(3).toString() == "szt.")
    {
        ui->r_szt->setChecked(true);
        ui->r_m->setChecked(false);
    }
    else
    {
        ui->r_szt->setChecked(false);
        ui->r_m->setChecked(true);
    }
}

void EdycjaTowaru::app()
{
    QString s;
    QSqlQuery q;
    s = "UPDATE towar SET nazwa='";
    s += ui->spec->text();
    s += "', cena_kat='";
    s += ui->cena->text();
    s += "', jednostka='";
    if(ui->r_m->isChecked())
        s += "mb.";
    else
        s += "szt.";
    s += "' WHERE id='";
    s += ui->kod->text();
    s += "'";
    EXEC(s);

    QMessageBox::information(this, "ok", "Dokonano edycji wpisu");
}


