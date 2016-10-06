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

#ifndef MERCHANDISELISTVIEW_H
#define MERCHANDISELISTVIEW_H

#include <QTableView>

class MerchandiseListView : public QTableView
{
    Q_OBJECT
public:
    explicit MerchandiseListView(QWidget *parent = 0);

    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void setModel(QAbstractItemModel *model);

protected:
    QPoint dragStartPosition;
    QHeaderView *header;
};

#endif // MERCHANDISELISTVIEW_H
