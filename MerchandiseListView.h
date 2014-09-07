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

protected:
    QPoint dragStartPosition;

};

#endif // MERCHANDISELISTVIEW_H
