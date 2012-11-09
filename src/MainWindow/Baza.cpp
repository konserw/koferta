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

#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QFileDialog>
#include <QSqlQuery>

#include "SyntaxKlient.h"
#include "SyntaxTowar.h"
#include "NowyKlient.h"
#include "NowyTowar.h"
#include "Database.h"
#include "EdycjaTowaru.h"
#include "EdycjaKlienta.h"
#include "Macros.h"

/* TOWAR */

void MainWindow::dodajTowar()
{
    cNowyTowar* nowyTowar = new cNowyTowar(this);
    nowyTowar->exec();
    delete nowyTowar;
}

void MainWindow::importTowar()
{
    QString s;
    s = QFileDialog::getOpenFileName(this, "Otwórz plik cennika", "", "Plik CSV (*.csv)");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć pliku z danymi cennika");
        if(!file.exists())DEBUG <<  "cennik nie iesnieje";
        else DEBUG <<  "cennik niedostępny";
        return;
    }

    mb->show();
    DEBUG <<  "wczytuje towar...";

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QStringList list;
    QSqlQuery q;
    QString sRead;

    for(unsigned i=0; !in.atEnd(); ++i)
    {
        sRead = in.readLine();
        list = sRead.split("|");
        if(list.size() < 3)
        {
            DEBUG <<  "syntax error, wczyt towar, line: " << i;
            syntax_towar sw(this, sRead);
            sw.exec();
            continue;
        }

        if(list.size() > 3 && (list.at(3) == "mb." || list.at(3) == "m" || list.at(3) == "metr"))
            s = "mb.";
        else
            s = "szt.";
        insert_towar(list.at(1), list.at(0), list.at(2), s);
    }
//    mb->hide();
    mb->accept();
    DEBUG <<  "koniec wczytywania";
}

void MainWindow::eksportTowar()
{
    QSqlQuery q;
    QString s;

    s = QFileDialog::getSaveFileName(this, "Zapisz plik cennika", "", "Plik CSV (*.csv)");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć plikudo zapisu");
        DEBUG <<  "Plik do zapisu towarów niedostępny";
        return;
    }

    mb->show();
    DEBUG <<  "zapis cennikow do pliku: " << s;

    QTextStream out(&file);
    out.setCodec("UTF-8");

    s = "SELECT id, nazwa, cena_kat, jednostka FROM towar";

    EXEC(s);

    while(q.next())
    {
        out << q.value(1).toString() << "|" << q.value(0).toString() << "|" << q.value(2).toString() << "|" << q.value(3).toString() << "\n";
    }

    mb->accept();
    DEBUG <<  "koniec zapisu cennika";
}

void MainWindow::edytujTowar()
{
    EdycjaTowaru* okno = new EdycjaTowaru(this);
    okno->exec();
    delete okno;
}


/* KLIENT */

void MainWindow::dodajKlient()
{
    cNowyKlient* nowyKlient = new cNowyKlient(this);
    nowyKlient->exec();
    delete nowyKlient;
}

void MainWindow::importKlient()
{
    QString s;
    s = QFileDialog::getOpenFileName(this, "Otwórz plik bazy klientów", "", "Plik CSV (*.csv)");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć pliku z danymi klientów");
        if(!file.exists()) DEBUG <<  "klienci nie iesnieje";
        else DEBUG <<  "klienci niedostępny";
        return;
    }

    mb->show();
    DEBUG <<  "wczytuje klientow...";

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QStringList list;
    QString sRead;

    for(unsigned i=0; !in.atEnd(); ++i)
    {
        sRead = in.readLine();
        list = sRead.split("|");
        if(list.size() < 8)
        {
            DEBUG <<  "syntax error, wczyt_klient line: " << i;
            syntax_klient sw(this, sRead);
            sw.exec();
            continue;
        }

        s = list.at(5);
        s += "<br>\n";
        s += list.at(6);
        s += " ";
        s += list.at(7);
        insert_klient(list.at(4), list.at(3), list.at(0), list.at(1), list.at(2), s);
    }

    DEBUG <<  "koniec wczytywania";
    mb->accept();
}

void MainWindow::eksportKlient()
{
    QSqlQuery q;
    QString s;

    s = QFileDialog::getSaveFileName(this, "Zapisz plik bazy klientów", "", "Plik CSV (*.csv)");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć pliku do zapisu");
        return;
    }

    mb->show();

    DEBUG <<  "zapis klientow do pliku: " << s;

    QTextStream out(&file);
    out.setCodec("UTF-8");

    s = "SELECT nazwisko, imie, short, full, tytul, adres FROM klient";

    EXEC(s);

    while(q.next())
    {
        for(int i=0; i<7; i++)
            out << q.value(i).toString() << "|";
        out << "\n";
    }

    mb->accept();
    DEBUG <<  "koniec zapisu klientow";
}

void MainWindow::edytujKlient()
{
    cEdycjaKlienta* okno = new cEdycjaKlienta(this);
    okno->exec();
    delete okno;
}
