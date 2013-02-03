/**
  Dialog umożliwiający ingerencję w strukturę bazy z poziomu roota
  **/
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

#ifndef ROOT_H
#define ROOT_H

#include <QDialog>
#include <QSqlQuery>

namespace Ui {
    class RootDialog;
}

class cRootDialog : public QDialog
{
    Q_OBJECT

public:
    explicit cRootDialog(QWidget *parent);
    ~cRootDialog();

public slots:
    void db_new();                                      //tworzy nową bazę danych
    void db_export();                                   //exportuje całą bazę do pliku - TODO
    void db_import();                                   //importuje całą bazę z pliku
    void db_del();                                      //usuwa istniejącą bazę
    void reset();                                       //resetuje tabele które zostały zaznaczone
    void addUser();                                     //uruchamia dialog dodający użytkownika
 //   void initUsers();                                   //inicjuje domyslnych uzytkowanikow

private:
    Ui::RootDialog *ui;

    void db_klient(bool ex);                            //resetuje/tworzy tabelę klientów
    void db_towar(bool ex);                             //resetuje/tworzy tabelę towarów
    void db_zapisane(bool ex);                          //resetuje/tworzy tabelę zapisanych ofert
    void db_users(bool ex);                             //resetuje/tworzy tabelę z danymi użytkowników

    void addu(QString name, QString pass, QString mail, QString adress, bool male, int nrOferty=0);
                                                        //dodaje uzytkownika z danymi z parametrow

    QSqlQuery q;
    QString s;

};

#endif // ROOT_H
