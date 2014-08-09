#ifndef TERMMODEL_H
#define TERMMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "TermItem.h"

/*!
 * \brief Model that contain possible terms for offer, payment etc.
 */
class TermModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TermModel(QObject *parent = 0);
    virtual ~TermModel();

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column, const QModelIndex &) const;

    void insert(TermItem *item);

protected:
    QList<TermItem*> m_list;
};

#endif // TERMMODEL_H
