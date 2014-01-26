#ifndef SZUKAJTOWARUMODEL_H
#define SZUKAJTOWARUMODEL_H

#include <QSortFilterProxyModel>

class MerchandiseSearchModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MerchandiseSearchModel(QObject *parent = 0);

    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    virtual int columnCount(const QModelIndex &/*index*/) const;
   // virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual void setFilter(const QString & filter);

protected:
    static const QString m_dateFmt;

    QVariant d(const QModelIndex &index, int col) const;
};

#endif // SZUKAJTOWARUMODEL_H
