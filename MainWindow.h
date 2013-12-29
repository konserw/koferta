/**
  Główne okno programu
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QSqlTableModel;
class QModelIndex;
class QTextDocument;
class QTableWidgetItem;
class User;
class QSqlRecord;
class QCalendarWidget;
class QDate;
class QPrinter;
class MerchandiseListModel;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();                              //aktualny użytkownik przekazywany jako parametr
    ~MainWindow();

    int ileTowaru(const QString&);                      //ilość danego towaru do wyświetlenia w wyborzeTowaru

    void uiReset();
public slots:
    void popWyborKlienta();
    void popWyborTowaru();

    void setTowar(const QSqlRecord&, int);              //dodawanie towarów do tabeli (wywoływane przez sygnał z dialogu dodajTowar)
    void loadOffer(const QSqlRecord &rec, const QSqlTableModel &mod);              //wczytuje ofertę o połączone z dialogiem wczytywanie

    //dodawanie opcji do kombosów
    void dostawaNew();
    void platnoscNew();
    void terminNew();
    void ofertaNew();

    //obsługa głównej tabeli
    void rabat();                                       //ustawia rabat dla wszystkich pozycji jednocześnie
    void del();                                         //usuwanie wiersza z tabeli

    //odświerzanie texteditów drugiej zakładce
    void dostawaRef(int);
    void platnoscRef(int);
    void terminRef(int);
    void ofertaRef(int);
    void zapytanieRef();
    void calChanged(const QDate&);                      //zmiana nastąpiła w kalendarzu, wprowadzenie jej do dateedita
    void checkNr(bool);
    void checkData(bool);
    void clientChanged(const QSqlRecord&);              //ustawia wybranego klienta - połączone z sygnałem z dialogu klient

    //opcje wydruku
    void pln_on();                                      //włącza przeliczanie euro na pln
    void pln_off();                                     //wyłącza przeliczanie euro na pln
    void chKurs(QString);                               //zmienia kurs wymiany euro->pln

    //wydruk
    void printPrev();                                   //podgląd wydruku
    void printPdf();                                    //export do pdf
    void printHtm();                                    //zapis jako htm
    void print(QPrinter *printer);                      //"drukowanie" dokumentu do podglądu lub pdf
    void makeDocument(QString *sDoc);                   //tworzy dokument jako kod htm w QTextDocument

/*menu*/
    //connect
    void connect();
    void disconnect();

    //menu oferta
    void nowa();
    void zapisz();
    void popLoadDialog();
    void nowyNumer();


    //klient
    void dodajKlient();                                 //wyświetla dialog dodający klienta - cDodajKlient
    void edytujKlient();                                //wyświetla dialog edycji klientów

    //towar
    void dodajTowar();                                  //wyświetlenie dialogu  dodającego towar - cDodajTowar

    //help
    void about();                                       //wyświetla informacje o programie
    //o qt - wbudowane

    void connectedAs(const User& user);

private:
    Ui::MainWindow *ui;

    //pomocnicze funkcje
    void setTitle(QString*);                            //ustawia tytuł okna
    void init();                                        //odblokowanie interfejsu i inicjacja tabeli
    void setMenusEnabled(bool en);

    //wewnętrzne zmienne
    QString* nr_oferty;
    QString* data;
    double kurs;
    bool pln;
    bool htm;

    QSqlTableModel* dostawaModel;
    QSqlTableModel* platnoscModel;
    QSqlTableModel* terminModel;
    QSqlTableModel* ofertaModel;

    QCalendarWidget* calendarWidget;
    QSqlRecord* klient;

    User* m_currentUser;
    MerchandiseListModel* m_towarModel;
};

#endif // MAINWINDOW_H
