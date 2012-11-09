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
#include "ui_edycjatowaru.h"

#include <QSqlQuery>
#include <QVariant>

#include "Macros.h"


EdycjaTowaru::EdycjaTowaru(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EdycjaTowaru)
{
    DEBUG << "Konstruktor cEdycjaTowaru";

    ui->setupUi(this);

    connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->radioButton, SIGNAL(pressed()), this, SLOT(ref2()));
    connect(ui->radioButton_ID, SIGNAL(pressed()), this, SLOT(ref2()));
    connect(ui->szukaj, SIGNAL(textChanged(QString)), this, SLOT(ref(QString)));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(app()));
    connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(click(int, int)));
    connect(ui->del, SIGNAL(clicked()), this, SLOT(del()));

    ui->tableWidget->setColumnWidth(1, 400);
    ui->tableWidget->setColumnWidth(0, 100);
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

    this->click(0, -1);
    ui->szukaj->clear();
}

void EdycjaTowaru::click(int row, int col)
{
    if(col == -1)
    {
        ui->kod->clear();
        ui->spec->clear();
        ui->cena->clear();
        return;
    }

    QString s;
    QSqlQuery q;

    s = "SELECT DISTINCT id, nazwa, cena_kat, jednostka FROM towar WHERE id='";
    s += ui->tableWidget->item(row, 0)->text();
    s += "'";
    EXEC(s);

    q.next();
    ui->kod->setText(q.value(0).toString());
    ui->spec->setText(q.value(1).toString());
    ui->cena->setText(q.value(2).toString());
    if(q.value(3).toString() == "szt.")
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


void EdycjaTowaru::ref2(){
    clear_table();
    ref(ui->szukaj->text());
}

void EdycjaTowaru::ref(QString text){
    DEBUG << "odswierzanie zawartosci tabeli dla frazy " << text;

    if(text == stary)
    {
        DEBUG << "fraza niezmieniona - pomijam";
        return;
    }

    if(text.size() == 0)
    {//brak tekstu ->czyszczenie
        DEBUG << "fraza pusta - czyszczenie";
        clear_table();
        return;
    }

   // if(stary.isEmpty()){//dla pierwszego odpalenia stara procedura
    if(ui->tableWidget->rowCount() == 0)
    {
        DEBUG << "brak wczesniejszych wyszukiwan -> stara procedura";
        stary_ref(text);
        return;
    }

    for(int i=0; i<stary.size(); i++)
    {
        if(stary[i].toLower() != text[i].toLower())//inne słowo -> czyszczenie + stara procedura
        {
            DEBUG << "zmieniona fraza - czyszczenie + stara procedura";
            clear_table();
            stary_ref(text);
            return;
        }
    }

    if(text.size() > stary.size())//zawężenie wyszukiwania
    {
        DEBUG << "usuwanie części wyników";
        int x;
        if(ui->radioButton->isChecked())x=1;//nazwa
        else x=0;//id
        for(int i=0; i<ui->tableWidget->rowCount(); ++i){
            QString cell = ui->tableWidget->item(i, x)->text();
            if(cell.size() < text.size()){
                ui->tableWidget->removeRow(i);
                i--;
                continue;
            }
            for(int j=0; j<text.size(); j++){
                if(cell[j].toLower() != text[j].toLower()){
                    ui->tableWidget->removeRow(i);
                    i--;
                    break;
                }
            }
        }
    }
    else
    {//rozszerzenie wyszukiwania
        DEBUG << "dodawanie nowych wyników";

        QSqlQuery q;
        QString s;

        s = "SELECT id, nazwa FROM towar WHERE ";
        if(ui->radioButton->isChecked()) s += "nazwa";
        else s += "id";
        s += " LIKE '";
        s += text;
        s += "%'";
        EXEC(s);

        int row = 0;
        while(q.next()){
            if(!ui->tableWidget->findItems(q.value(0).toString(), Qt::MatchFixedString).isEmpty())
                continue;

            ui->tableWidget->insertRow(row);

            QTableWidgetItem* item;
            for(int i=0; i<2; ++i)
            {
                item = new QTableWidgetItem(q.value(i).toString());
                ui->tableWidget->setItem(row, i, item);
            }
        }
    }
    stary = text;
    DEBUG << "koniec odswierzania dla " << text;
}

void EdycjaTowaru::stary_ref(QString text){
    QString s;
    QSqlQuery q;

    s = "SELECT id, nazwa FROM towar WHERE ";
    if(ui->radioButton->isChecked()) s += "nazwa";
    else s += "id";
    s += " LIKE '";
    s += text;
    s += "%'";
    EXEC(s);

    int row = 0;
    while(q.next()){
        ui->tableWidget->insertRow(row);

        QTableWidgetItem* item;
        for(int i=0; i<2; ++i)
        {
            item = new QTableWidgetItem(q.value(i).toString());
            ui->tableWidget->setItem(row, i, item);
        }

        row++;
    }
    stary = text;
}
void EdycjaTowaru::clear_table(){
    while(ui->tableWidget->rowCount())
        ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
}

EdycjaTowaru::~EdycjaTowaru()
{
    delete ui;
}
