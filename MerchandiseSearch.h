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

#ifndef SZUKAJTOWARU_H
#define SZUKAJTOWARU_H

#include <QDialog>

class MerchandiseSearchModel;

namespace Ui {
    class MerchandiseSearch;
}

/*!
 * \brief Abstrakcyjna klasa okna dialogowego do szukania towarów
 */
class MerchandiseSearch : public QDialog
{
    Q_OBJECT
    
public:
    explicit MerchandiseSearch(MerchandiseSearchModel *model, QWidget *parent = 0);
    virtual ~MerchandiseSearch() = 0;

private slots:
    void ref(const QString&);
    void ref2();

protected:
    MerchandiseSearchModel* m_model;

//private:
    Ui::MerchandiseSearch *ui;

};

#endif // SZUKAJTOWARU_H
