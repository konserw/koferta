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

#ifndef WCZYTYWANIE_H
#define WCZYTYWANIE_H

class QString;
class QSqlRecord;
class LoadDialogMerchandiseListModel;

#include <QDialog>

namespace Ui {
    class LoadDialog;
}
/*!
 * \brief Dialog wyboru oferty do wczytania
 */
class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadDialog(QWidget *parent = 0);
    ~LoadDialog();

    int selectedOfferId() const;

private:
    Ui::LoadDialog *ui;

    LoadDialogMerchandiseListModel* model;
};

#endif // WCZYTYWANIE_H
