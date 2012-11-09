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

#include "WyborTowaru.h"
#include "ui_WyborTowaru.h"

#include <QSpinBox>
#include <QSqlQuery>
#include "Macros.h"

cWyborTowaru::cWyborTowaru(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WyborTowaru)
{
    ui->setupUi(this);

    connect(ui->but_add, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->but_canc, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->radioButton, SIGNAL(pressed()), this, SLOT(ref2()));
    connect(ui->radioButton_ID, SIGNAL(pressed()), this, SLOT(ref2()));
    connect(ui->szukaj, SIGNAL(textChanged(QString)), this, SLOT(ref(QString)));

    ui->tableWidget->setColumnWidth(2, 500);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(3, 80);
    ui->tableWidget->setColumnWidth(0, 50);
}
cWyborTowaru::~cWyborTowaru()
{
    delete ui;
}
void cWyborTowaru::ref2()
{
    clear_table();
    ref(ui->szukaj->text());
}

void cWyborTowaru::ref(QString text)
{
    if(text == stary)return;//bez zmian

    if(text.size() == 0)//brak tekstu ->czyszczenie
    {
        clear_table();
        return;
    }

    if(stary.isEmpty()){//dla pierwszego odpalenia stara procedura
        stary_ref(text);
        stary = text;
        return;
    }

    bool flag = true;
    for(int i=0; i<stary.size(); i++){
        if(stary[i].toLower() != text[i].toLower()){
            flag = false;
            break;
        }
    }
    if(flag == false){//inne słowo -> czyszczenie + stara procedura
        clear_table();
        stary_ref(text);
        stary = text;
        return;
    }
    if(text.size() > stary.size())//zawężenie wyszukiwania
    {
        int x;
        if(ui->radioButton->isChecked())x=2;//nazwa
        else x=1;//id
        for(int i=0; i<ui->tableWidget->rowCount(); ++i)
        {
            QString cell = ui->tableWidget->item(i, x)->text();
            if(cell.size() < text.size())
            {
                ui->tableWidget->removeRow(i);
                i--;
                continue;
            }
            for(int j=0; j<text.size(); j++)
            {
                if(cell[j].toLower() != text[j].toLower())
                {
                    ui->tableWidget->removeRow(i);
                    i--;
                    break;
                }
            }
        }
    }
    else//rozszerzenie wyszukiwania
    {
        QSqlQuery q;
        QString s;
        int row = 0;
        QSpinBox* spin;
        QTableWidgetItem* item;

        s = "SELECT * FROM towar WHERE ";
        if(ui->radioButton->isChecked()) s += "nazwa";
        else s += "id";
        s += " LIKE '";
        s += text;
        s += "%'";

        EXEC(s);

        while(q.next())
        {
            if(!ui->tableWidget->findItems(q.value(0).toString(), Qt::MatchFixedString).isEmpty())
                continue;

            ui->tableWidget->insertRow(row);

            spin = new QSpinBox;
            spin->setMinimum(0);
            spin->setMaximum(9999);
            ui->tableWidget->setCellWidget(row, 0, spin);

            for(int i=0; i<3; ++i)
            {
                item = new QTableWidgetItem(q.value(i).toString());
                ui->tableWidget->setItem(row, i+1, item);
            }
        }
    }
    stary = text;
}

void cWyborTowaru::stary_ref(QString text)
{
    QString s;
    QSqlQuery q;
    int row = 0;
    QTableWidgetItem* item;
    QSpinBox* spin;

    s = "SELECT * FROM towar WHERE ";
    if(ui->radioButton->isChecked()) s += "nazwa";
    else s += "id";
    s += " LIKE '";
    s += text;
    s += "%';";

    EXEC(s);

    while(q.next()){
        ui->tableWidget->insertRow(row);

        spin = new QSpinBox;
        spin->setMinimum(0);
        spin->setMaximum(9999);
        ui->tableWidget->setCellWidget(row, 0, spin);

        for(int i=0; i<3; ++i)
        {
            item = new QTableWidgetItem(q.value(i).toString());
            ui->tableWidget->setItem(row, i+1, item);
        }

        row++;
    }
    stary = text;
}
void cWyborTowaru::clear_table()
{
    while(ui->tableWidget->rowCount())
        ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
}
void cWyborTowaru::ok()
{
    QHash<QString, int> hasz;
    int v;
    QSpinBox* spin;

    for(int i=0; i<ui->tableWidget->rowCount(); ++i)
    {
        spin = qobject_cast<QSpinBox*>(ui->tableWidget->cellWidget(i, 0));
        v = spin->value();
        if(v > 0)
            hasz.insert(ui->tableWidget->item(i, 1)->text(), v);
    }
    emit sig(hasz);

    this->close();
}
