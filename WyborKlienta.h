/**
  Dialog cWyborKlienta umożliwiający wybór klienta z bazy dla którego adresowana jest oferta
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

#ifndef KLIENT_H
#define KLIENT_H

#include <QDialog>
class cNowyKlient;

namespace Ui {
    class WyborKlienta;
}

class cWyborKlienta : public QDialog
{
    Q_OBJECT

public:
    explicit cWyborKlienta(QWidget *parent = 0);        //inicjacja ui
    ~cWyborKlienta();

public slots:
    void ok();

signals:
    void id_klienta(int);                           //sygnał przekazujący wybranego lienta do MainQindow

private:
    QString* s;
    Ui::WyborKlienta *ui;
};

#endif // KLIENT_H

