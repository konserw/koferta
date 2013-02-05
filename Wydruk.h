/**
  Objekt realizujący funkcje eksportu bazy
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

#ifndef WYDRUK_H
#define WYDRUK_H

#include <QWidget>


class QPrinter;
class QString;
class QPrintPreviewDialog;
class QFont;
class QTextDocument;
class cUser;

namespace Ui {
    class MainWindow;
}

class cWydruk: public QWidget
{
    Q_OBJECT

public:
    explicit cWydruk(Ui::MainWindow* _ui, QString* data, int* klient, QString* numer);
    ~cWydruk();

public slots:
    void prev();                                        //podgląd wydruku
    void do_pdf();                                      //export do pdf
    void do_htm();                                      //zapis jako htm

private slots:
    void print(QPrinter *printer);                      //"drukowanie" dokumentu do podglądu lub pdf

signals:
    void zapisz();                                      //sygnał wymuszający zapiosanie oferty w bazie przy każdym exporcie

private:
    QString* sDoc;                                      //Dokument oferty
    Ui::MainWindow* ui;
    bool htm;                                           //true jeżeli ma być zapis do htm

    void make();                                        //tworzy dokument jako kod htm zapisany w sDoc

    static const int w = 745;                           //szerokosc szkieletu dokumentu
    static const int d = (w-5)/2;                       //szerokość kolumny w szkielecie
    int z[8];                                           //szerokosc komorek tabeli
    static const int dw = 140;                          //szerokosc pierwszej kolumny w szkielecie poniżej tabeli
    static const int margin = 5;                        //szerokość marginesu

    QPrinter* printer;
    QPrintPreviewDialog* preview;
    QFont* font;
    QTextDocument* doc;

    QString* _data;                                     //data sporządzenia oferty
    int* _klient;                                        //id klienta
    QString* _numer;                                    //nr. oferty
};

#endif // WYDRUK_H
