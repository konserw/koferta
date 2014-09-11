#include "MerchandiseListView.h"
#include "MerchandiseListDelegate.h"
#include "MerchandiseListModel.h"

#include <QtPrintSupport>

MerchandiseListView::MerchandiseListView(QWidget *parent) :
    QTableView(parent)
{
    setItemDelegate(new MerchandiseListDelegate(this));
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAcceptDrops(true);

    setSortingEnabled(true);
    this->horizontalHeader()->setSortIndicatorShown(false);
}

void MerchandiseListView::dragEnterEvent(QDragEnterEvent *event)
 {
    if(event->source() == this)
        event->acceptProposedAction();
}

void MerchandiseListView::dropEvent(QDropEvent *event)
{
    QModelIndex indexFrom = indexAt(dragStartPosition);
    QModelIndex indexTo = indexAt(event->pos());

    dynamic_cast<MerchandiseListModel*>(this->model())->moveRow(QModelIndex(), indexFrom.row(), QModelIndex(), indexTo.row());

    event->acceptProposedAction();
}

void MerchandiseListView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();

    QTableView::mousePressEvent(event);
}
