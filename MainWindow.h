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
#include <QHash>
#include <QDate>

class QTextDocument;
class QStringList;
class QTableWidgetItem;
class cWyborKlienta;
class WyborTowaru;
class QMessageBox;
class cLoadDialog;
class QSqlDatabase;
class QSqlQuery;
class cWydruk;
class cUser;


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(cUser*);                        //aktualny użytkownik przekazywany jako parametr
    ~MainWindow();

    int ileTowaru(const QString&);                      //ilość danego towaru do wyświetlenia w wyborzeTowaru

public slots:    
    void setTowar(QString, int);                        //dodawanie towarów do tabeli (wywoływane przez sygnał z dialogu dodajTowar)
    void setKlient(int id);                             //ustawia wybranego klienta - połączone z sygnałem z dialogu klient

    void wczytaj_oferte(QString);                       //wczytuje ofertę o zadanym id, połączone z dialogiem wczytywanie

    //obsługa głównej tabeli
    void change(QTableWidgetItem*);                     //wywoływane przeliczenia wartości kosztu i sumy w przypadku zmiany którejś wartości w tabeli
    void clear();                                       //czyszczenie tabeli
    void rabat();                                       //ustawia rabat dla wszystkich pozycji jednocześnie
    void del();                                         //usuwanie wiersza z tabeli


    //odświerzanie texteditów po zmianie w comboboxie  w drugiej zakładce
    void ref(QString);
    void ref2(QString);
    void ref3(QString);
    void ref4(QString);
    //zmiany daty
    void calChanged();                                  //zmiana nastąpiła w kalendarzu, wprowadzenie jej do dateedita
    void dateChanged(QDate);                            //zmiana w dateedicie, wprowadzenie jej do edita

    //opcje wydruku
    void pln_on();                                      //włącza przeliczanie euro na pln
    void pln_off();                                     //wyłącza przeliczanie euro na pln
    void chKurs(QString);                               //zmienia kurs wymiany euro->pln

/*menu*/
    //menu plik
    void nowa();
    void zapisz();
    void wczytaj();
    void nowyNumer();
    //exit zaimplementowany

    //klient
    void dodajKlient();                                 //wyświetla dialog dodający klienta - cDodajKlient
    void importKlient();                                //import klientow z pliku csv
    void eksportKlient();                               //ekport bazy klientów do pliku .csv
    void edytujKlient();                                //wyświetla dialog edycji klientów

    //towar
    void dodajTowar();                                  //wyświetlenie dialogu  dodającego towar - cDodajTowar
    void importTowar();                                 //import cennika z pliku .csv
    void eksportTowar();                                //eksport cennika do pliku .csv
    void edytujTowar();                                 //wyświetla dialog edycji towarów
    //help
    void about();                                       //wyświetla informacje o programie
    //o qt - wbudowane

private:
    Ui::MainWindow *ui;

    cWyborKlienta* kw;
    WyborTowaru* tw;
    QMessageBox* mb;
    cLoadDialog* ww;
    cWydruk* wyd;

    //obsługa głównej tabeli
    void sum();                                         //przelicza sumę wszystkich pozycji
    void przelicz(uint);                                //przelicza koszt towaru z zadanego wiersza
    double ev(unsigned row, unsigned col);              //odczytuje wartość numeryczną z komórki o podanych koordynatach
    void tabupd();                                      //przeliczenie wartości w całej tabeli po zmianie waluty

    //pomocnicze funkcje
    void setTitle(QString*);                             //ustawia tytuł okna
    void init();                                        //odblokowanie interfejsu i inicjacja tabeli

    //domyślne wypełnienia textboxów z 2 zakładki wraz z odpowiednimi etykietami do comboboxów
    QHash<QString, QString>* dostawa;
    QHash<QString, QString>* platnosc;
    QHash<QString, QString>* termin;
    QHash<QString, QString>* oferta;
   // QHash<QString, QString>* zapytanie;

    //wewnętrzne zmienne
    int id_klienta;
    QString* nr_oferty;
    QString* data;
    cUser* u;
    double kurs;
    bool pln;

};

#endif // MAINWINDOW_H
