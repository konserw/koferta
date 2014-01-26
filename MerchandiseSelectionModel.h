#ifndef TOWARPROXY_H
#define TOWARPROXY_H

#include "MerchandiseSearchModel.h"
#include <QHash>

class MerchandiseSelectionModel : public MerchandiseSearchModel
{
    Q_OBJECT

public:  
    MerchandiseSelectionModel(const QHash<int, double>& hash, QObject *parent = nullptr);

    virtual int columnCount(const QModelIndex &) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual void setHash(const QHash<int, double>& hash);

signals:
    void itemCountChanged(int id, double count);

protected:
    QHash<int, double> m_hash;

};

#endif // TOWARPROXY_H
