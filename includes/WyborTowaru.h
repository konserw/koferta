/**
  Dialog cWyborTowaru służy do wyszukiwania towarów w bazie oraz wyboru towarów dodawanych do oferty
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

#ifndef DODAJTOWAR_H
#define DODAJTOWAR_H

#include <QDialog>
#include <QHash>

namespace Ui {
    class WyborTowaru;
}

class cWyborTowaru : public QDialog
{
    Q_OBJECT

public:
    explicit cWyborTowaru(QWidget *parent = 0);         //inicjalizacja ui oraz połączenie slotów
    ~cWyborTowaru();

public slots:
    void ref2();                                        //slot wywoływany przy zmianie opji wyszukiwania, wywołuje ref(QString) z odpowiednim parametrem
    void ref(QString);                                  //slot wywoływany w momencie zmiany w polu tekstowym wyszukiwarki. Pobiera z bazy danych informacje o towarach spełniających kryteria wyszukiwania i wstawia do tabeli widocznej dla użytkownika
    void ok();                                          //wywoływany przyciskiem dodaj, przetwarzanie informacji z tabeli na hash i wysłanie sygnału

signals:
    void sig(QHash<QString, int>);                      //sygnał przenoszący informacje o wybranyuch towarach i ich ilości do MainWindow

private:
    Ui::WyborTowaru *ui;
    QString stary;                                      //stara wartość wyszukiwania przechowywana do porównania z wartością aktualną
    void stary_ref(QString);                            //wykorzystywane wewnętrznie przez ref(QString)
    void clear_table();                                 //czyści całą tabelę
};

#endif // DODAJTOWAR_H
