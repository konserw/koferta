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
    
#ifndef SZUKAJKLIENTA_H
#define SZUKAJKLIENTA_H

#include <QWidget>

class QSqlTableModel;
class QSqlRecord;
class QModelIndex;

namespace Ui {
    class SzukajKlienta;
}

class SzukajKlienta : public QWidget
{
    Q_OBJECT
    
public:
    explicit SzukajKlienta(QWidget *parent = 0);
    ~SzukajKlienta();

public slots:
    void ref(const QString&);
    void ref2();
    void select(const QModelIndex&);

signals:
    void selectionChanged(const QSqlRecord&);

private:
    QSqlTableModel* model;
    Ui::SzukajKlienta *ui;
    int id;
};

#endif // SZUKAJKLIENTA_H
