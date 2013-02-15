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
#include <QSqlQuery>
#include <QString>
#include "Database.h"
#include "Macros.h"

void insert_towar(const QString& id, const QString& nazwa, double cena, const QString& jednostka)
{
    QSqlQuery q;
    QString s;

    s = QString("SELECT nazwa FROM towar WHERE id='%1'").arg(id);
    EXEC(s);
    if(q.next())
    {
        DEBUG <<  "Towar o id: " << id << " istnieje w bazie, nastapi jego nadpisanie!";
        s = QString("DELETE FROM towar WHERE id='%1'").arg(id);
        EXEC(s);
    }

    s = QString("INSERT INTO towar (id, nazwa, cena, jednostka) VALUES ('%1', '%2', '%3', '%4')").arg(id, nazwa, QString::number(cena), jednostka);
   // DEBUG <<  "Wstawianie towaru, id: " << id;
    EXEC(s);
}

void insert_klient(const QString& skrot, const QString& full, const QString& tytul, const QString& imie, const QString& nazwisko, const QString& adres)
{
    QString s;
    QSqlQuery q;
    s = "INSERT INTO klient (short, full, tytul, imie, nazwisko, adres) VALUES ('";
    s += skrot;
    s += "', '";
    s += full;
    s += "', '";
    s += tytul;
    s += "', '";
    s += imie;
    s += "', '";
    s += nazwisko;
    s += "', '";
    s += adres;
    s += "');";
 //   DEBUG <<  "Wstawianie klienta, nazwisko: " << nazwisko;
    EXEC(s);
}

void insert_zapisane(const QString& nr_oferty, int id_klienta, const QString& data, int uid, const QString& zapytanie_data, const QString& zapytanie_nr, int dostawa, int termin, int platnosc, int oferta, const QString& uwagi)
{
    QString s;
    QSqlQuery q;
    s = QString("DELETE FROM zapisane WHERE nr_oferty = '%1'").arg(nr_oferty);
    EXEC(s);

    s = QString("DELETE FROM zapisane_towary WHERE nr_oferty = '%1'").arg(nr_oferty);
    EXEC(s);

    s = "INSERT INTO zapisane "
        "(nr_oferty, id_klienta, data, uid, zapytanie_data, zapytanie_nr, dostawa, termin, platnosc, oferta, uwagi)";
    s += QString(" VALUES ('%1', %2, '%3', %4, '%5', '%6', %7, %8, %9").arg(nr_oferty, QString::number(id_klienta), data, QString::number(uid), zapytanie_data, zapytanie_nr, QString::number(dostawa), QString::number(termin), QString::number(platnosc));
    s += QString(", %1, '%2')").arg(QString::number(oferta), uwagi);
//    DEBUG <<  "Zapisywanie ofery nr: " << nr_oferty;
    EXEC(s);
}

void insert_zapisane_towary(const QString& nr_oferty, const QString& kod, double ilosc, double rabat)
{
    QString s;
    QSqlQuery q;
    s = QString("INSERT INTO zapisane_towary (nr_oferty, kod, ilosc, rabat) VALUES ('%1', '%2', %3, %4)").arg(nr_oferty, kod, QString::number(ilosc), QString::number(rabat));
//    DEBUG <<  "Zapisywanie oferty nr: " << nr_oferty << " - kod: " << kod;
    EXEC(s);
}
