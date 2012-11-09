/**
  dialog umożliwiający zalogowanie się do systemu
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

#ifndef LOG_H
#define LOG_H

#include <QDialog>

class QStringList;
class QSqlDatabase;
class QNetworkAccessManager;
class QNetworkReply;
class cUser;

namespace Ui {
    class Logowanie;
}

class cLogowanie : public QDialog
{
    Q_OBJECT

public:
    explicit cLogowanie(cUser**);                       //konstruktor inicjalizujący elementy interface, user** służy do przekazania zwrotnego danych wybranego użytkownika
    ~cLogowanie();

public slots:
    void ok();                                          //slot obsługujący przycisk ok - sprawdza zgodność skrótu wpisanego hasła ze skrótem zapisanym w klasie user i tworzy połączenie z bazą MySQL (klasa QSqlDatabase)
    void add();                                         //slot obsługujący dodawanie dodatkowych adresów IP bazy danych, zarówno do listy jak i do pliku host
    void hostChanged(QString);                          //slot odświerzający listę dostępnych użytkowników po zmianie hosta
    void downloadFinished(QNetworkReply*);              //koniec pobierania aktualizacji

private:
    Ui::Logowanie *ui;

    QStringList* hosts;                                 //lista dostępnych hostów, generowana na podstawie pliku tekstowego
    QPixmap* p;                                         //bufor grafiki loga systemu kOferta
    cUser *u, **us;                                     //wskaźnik podwójny zapamiętany z konstruktora i roboczy wskaźnik
    QSqlDatabase* d;                                    //obiekt połączenia z bazą
    QString* host;                                      //nazwa hosta bazy danych z którą aplikacja się łączy

    QNetworkAccessManager* manager;                     //objekt odpowiedzialny za pobieranie aktualizacji
    double ver;                                         //wersja programu pobieranego z serwera

};
#endif // LOG_H
