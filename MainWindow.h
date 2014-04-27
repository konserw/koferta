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
/*!
 * \brief Główne okno programu
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

public slots:
    void popWyborKlienta();
    void popWyborTowaru();

    ///wczytuje ofertę o połączone z dialogiem wczytywanie
    void loadOffer(const QString &offerId);

    //dodawanie opcji do kombosów
    void dostawaNew();
    void platnoscNew();
    void terminNew();
    void ofertaNew();

    //obsługa głównej tabeli
    ///ustawia rabat dla wszystkich pozycji jednocześnie
    void rabat();
    ///usuwanie wiersza z tabeli
    void del();

    //odświerzanie texteditów drugiej zakładce
    void dostawaRef(int);
    void platnoscRef(int);
    void terminRef(int);
    void ofertaRef(int);
    void zapytanieRef();
    ///zmiana nastąpiła w kalendarzu, wprowadzenie jej do dateedita
    void calChanged(const QDate&);
    void checkNr(bool);
    void checkData(bool);
    ///ustawia wybranego klienta - połączone z sygnałem z dialogu klient
    void clientChanged(const QSqlRecord&);

    //opcje wydruku
    ///włącza przeliczanie euro na pln
    void pln_on();
    ///wyłącza przeliczanie euro na pln
    void pln_off();

    //wydruk
    ///podgląd wydruku
    void printPrev();
    ///export do pdf
    void printPdf();
    ///zapis jako htm
    void printHtm();
    ///"drukowanie" dokumentu do podglądu lub pdf
    void print(QPrinter *printer);
    ///tworzy dokument jako kod htm do QTextDocument
    void makeDocument(QString *sDoc);

/*menu*/
    //kOferta
    void databaseConnect();
    void databaseDisconnect();
    void changeSettings();

    //menu oferta
    void nowa();
    void zapisz();
    void popLoadDialog();
    void nowyNumer();


    //baza
    ///wyświetla dialog dodający klienta
    void dodajKlient();
    ///wyświetla dialog edycji klientów
    void edytujKlient();
    ///wyświetla dialog dodający towar
    void dodajTowar();

    //help
    ///wyświetla informacje o programie
    void about();
    //o qt - wbudowane

    void connectedAs(const User& user);

protected:
    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    //pomocnicze funkcje
    ///ustawia tytuł okna
    void setTitle(QString*);
    ///odblokowanie interfejsu i inicjacja tabeli
    void init();
    void setMenusEnabled(bool en);
    void uiReset();

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
