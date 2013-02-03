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

#include "RootDialog.h"
#include "ui_RootDialog.h"
#include "User.h"
#include "NowyUser.h"
#include "SHA1.h"
#include "Database.h"
#include "Macros.h"

#include <QSqlQuery>
#include <QFile>
#include <QFileDialog>
#include <QStringList>

cRootDialog::cRootDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RootDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->reset, SIGNAL(clicked()), this, SLOT(reset()));    
    connect(ui->addUser, SIGNAL(clicked()), this, SLOT(addUser()));

    connect(ui->db_new, SIGNAL(clicked()), this, SLOT(db_new()));
    connect(ui->db_import, SIGNAL(clicked()), this, SLOT(db_import()));
    connect(ui->db_export, SIGNAL(clicked()), this, SLOT(db_export()));
    connect(ui->db_del, SIGNAL(clicked()), this, SLOT(db_del()));
}

cRootDialog::~cRootDialog()
{
    delete ui;
}
void cRootDialog::db_del()
{
    QSqlQuery q;
    QString s = "DROP DATABASE kOferta";
    EXEC(s);

    QMessageBox::information(this, "Baza danych usunięta", "Usunięto bazę danych.");
}

void cRootDialog::db_users(bool ex)
{
    if(ex)
    {
        EXEC("use mysql");
        EXEC("DELETE FROM user WHERE User NOT LIKE 'root%'");
        EXEC("use kOferta");
        EXEC("DROP TABLE users");
    }else
        EXEC("use kOferta");

    EXEC("CREATE TABLE users("
           "uid INT AUTO_INCREMENT PRIMARY KEY, "
           "name TEXT, "
           "mail TEXT, "
           "adress TEXT, "
           "male BOOL, "
           "nrOferty INT"
           ") DEFAULT CHARSET=utf8");

    s = "GRANT CREATE USER ON *.* TO 'kOferta_GetUsers'@'%' IDENTIFIED BY '" GET_PASS "'";
#ifndef NOSSL
    s += " REQUIRE SSL";
#endif
    EXEC(s);


    QString foo[4];
    foo[0] = "SELECT";
    foo[1] = "INSERT";
    foo[2] = "UPDATE";
    foo[3] = "DELETE";

    for(int j=0; j<4; j++)
    {
        s = "GRANT ";
        s += foo[j];
        s += " ON kOferta.* TO 'kOferta_GetUsers' with grant option";

        EXEC(s);
    }
}

void cRootDialog::db_klient(bool ex)
{ 
    EXEC("USE kOferta");
    if(ex)EXEC("DROP TABLE klient");

    EXEC("CREATE TABLE klient("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "short TEXT,"
            "full TEXT,"
            "tytul TEXT,"
            "imie TEXT,"
            "nazwisko TEXT,"
            "adres TEXT"
            ") DEFAULT CHARSET=utf8");
}

void cRootDialog::db_towar(bool ex)
{
    EXEC("USE kOferta");
    if(ex)EXEC("DROP TABLE towar");

    EXEC("CREATE TABLE towar("
            "id VARCHAR(20) PRIMARY KEY,"
            "nazwa TEXT,"
            "cena_kat TEXT,"
            "jednostka ENUM('szt.', 'mb.')"
            ") DEFAULT CHARSET=utf8");
}

void cRootDialog::db_zapisane(bool ex)
{
    if(ex)
    {
        EXEC("DROP TABLE zapisane");
        EXEC("DROP TABLE zapisane_towary");
    }
    EXEC("CREATE TABLE zapisane("
           "nr_oferty VARCHAR(10) PRIMARY KEY, "
           "id_klienta INT, "
           "data TEXT, "
           "uid INT, "
           "zapytanie TEXT, "
           "dostawa TEXT, "
           "termin TEXT, "
           "platnosc TEXT,"
           "oferta TEXT"
           ") DEFAULT CHARSET=utf8");
    EXEC("CREATE TABLE zapisane_towary ("
          "nr_oferty VARCHAR(10), "
          "kod VARCHAR(20), "
          "ilosc REAL, "
          "rabat REAL"
          ") DEFAULT CHARSET=utf8");
}

void cRootDialog::db_new()
{
    EXEC("SET NAMES 'utf8'");
    EXEC("CREATE DATABASE kOferta");
    EXEC("DEFAULT CHARACTER SET utf8");

    this->db_towar(false);
    this->db_klient(false);
    this->db_zapisane(false);
    this->db_users(false);

    QMessageBox::information(this, "Baza zrekonfigurowana", "Baza została zrekonfigurowana.");
}

void cRootDialog::reset()
{
    if(ui->towar->isChecked())this->db_towar(true);
    if(ui->klient->isChecked())this->db_klient(true);
    if(ui->zapisane->isChecked())this->db_zapisane(true);
    if(ui->users->isChecked())this->db_users(true);


    QMessageBox::information(this, "reset", "Reset wybranych tabel zakończony");
}

void cRootDialog::addUser()
{
    cNowyUser* nu = new cNowyUser(this);
    nu->exec();
    delete nu;
}

void cRootDialog::db_import()
{
    s = QFileDialog::getOpenFileName(this, "Wczytaj plik bazy danych", "", "");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć pliku do odczytu");
        return;
    }

    DEBUG <<  "importowanie bazy z pliku: " << s;

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QStringList l;

    int tryb=-1, i=0;
    QString tmp;
    while(!in.atEnd())
    {
        ++i;
        s = in.readLine();
        if(s.isEmpty())continue;
        if(s=="###klient")tryb=0;
        else if(s=="###towar")tryb=1;
        else if(s=="###zamow")tryb=2;
        else if(s=="###zapisane")tryb=3;
        else if(s=="###zapisane_towary")tryb=4;
        else
        {
            l=s.split('|', QString::KeepEmptyParts);
            DEBUG <<  "tryb: " << tryb << " wielkosc listy: " << l.size() << " i: " << i;
            switch(tryb)
            {
            case 0:
                tmp = l.at(5);
                tmp.replace("<br>", "<br>\n");
                insert_klient(l.at(0), l.at(1), l.at(2), l.at(3), l.at(4), tmp);
                break;
            case 1:
                tmp = l.at(3);
                if(tmp == "metr" || tmp == "mb." || tmp == "m")
                    tmp = "mb.";
                else
                    tmp = "szt.";
                insert_towar(l.at(0), l.at(1), l.at(2), tmp);
                break;
            case 2:
                s = "UPDATE users SET nrOferty=";
                s += l.at(0);
                s += " WHERE uid=";
                s += l.at(1);
                EXEC(s);
                DEBUG <<  "upd_users: " << s;
                break;
            case 3:
                insert_zapisane(l.at(0), l.at(1).toInt(), l.at(2), l.at(3), l.at(4), l.at(5), l.at(6), l.at(7), l.at(8));
                break;
            case 4:
                insert_zapisane_towary(l.at(0), l.at(1), l.at(2), l.at(3));
                break;
            default:
                DEBUG <<  "nieznany tryb!";
            }
        }
    }

    DEBUG <<  "koniec importu bazy";
    QMessageBox::warning(this, "importowanie", "importowanie zakończone");
}

void cRootDialog::db_export()
{
    s = QFileDialog::getSaveFileName(this, "Zapis pliku bazy danych", "", "");
    if(s.isEmpty())return;

    QFile file(s);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error","Nie udało się otworzyć pliku do zapisu");
        return;
    }

    DEBUG <<  "eksport bazy do pliku: " << s;

    QTextStream out(&file);
    out.setCodec("UTF-8");

    out << "###klient\n";

    EXEC("SELECT short, full, tytul, imie, nazwisko, ulica, miasto FROM klient");
    while(q.next())
    {
        for(int i=0; i<5; ++i)
            out << q.value(i).toString() << "|";
        out << q.value(5).toString() << "<br>" << q.value(6).toString();
        out << "\n";
    }

    out << "###towar\n";

    EXEC("SELECT id, nazwa, cena_kat, jednostka FROM towar");
    while(q.next())
    {
        for(int i=0; i<4; ++i)
            out << q.value(i).toString() << "|";
        out << "\n";
    }

    out << "###zamow\n";
    EXEC("SELECT u1, u2, u3, u4 FROM zamow");
    q.next();
    for(int i=1; i<5; ++i)
        out << q.value(i).toString() << "|" << i << "\n";

    out << "###zapisane\n";

    EXEC("SELECT nr_oferty, id_klienta, data, uid, zapytanie, dostawa, termin, platnosc, oferta FROM zapisane");
    while(q.next())
    {
      for(int i=0; i<9; ++i)
          out << q.value(i).toString() << "|";
      out << "\n";
    }

    out << "###zapisane_towary\n";

    EXEC("SELECT nr_oferty, kod, ilosc, rabat FROM zapisane_towary");
    while(q.next())
    {
        for(int i=0; i<4; ++i)
            out << q.value(i).toString() << "|";
        out << "\n";
    }

    DEBUG <<  "koniec eksportu bazy";
    QMessageBox::warning(this, "eksport", "eksportowanie zakończone");

}
