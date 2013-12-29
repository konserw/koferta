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

#ifndef WYBORTOWARU_H
#define WYBORTOWARU_H

#include "MerchandiseSearch.h"

class QSqlRecord;
class QSqlTableModel;
class QModelIndex;

/*!
 * \brief Klasa umożliwiająca dodanie towaru do oferty
 */
class MerchandiseSelection : public MerchandiseSearch
{
    Q_OBJECT
    
public:
    explicit MerchandiseSelection(const QHash<int, double>& hash = QHash<int, double>(), QWidget *parent = 0);
    ~MerchandiseSelection();

signals:
    void itemCountChanged(int id, double count);

protected:
    QSqlTableModel* m_merchandiseModel;
};

#endif // WYBORTOWARU_H
