#include <QDebug>
#include "TermModel.h"

TermModel::TermModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

TermModel::~TermModel()
{
    qDeleteAll(m_list);
}

int TermModel::rowCount(const QModelIndex &) const
{
    return m_list.count();
}

int TermModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant TermModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole || !index.isValid() || (index.row() > m_list.count()))
        return QVariant();

    TermItem* t = m_list.at(index.row());
    switch(index.column())
    {
    case 0:
        return t->id();
    case 1:
        return t->shortDesc();
    case 2:
        return t->longDesc();
    }

    qWarning() << "View requested non-existant column, number:" << index.column();
    return QVariant();
}

QVariant TermModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Vertical)
        return QString::number(section);

    switch (section)
    {
    case 0:
        return tr("Id");
    case 1:
        return tr("Krótki opis");
    case 2:
        return tr("Tekst opcji");
    }

    qWarning() << "View requested non-existant column, number:" << section;
    return QVariant();
}

Qt::ItemFlags TermModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

QModelIndex TermModel::index(int row, int column, const QModelIndex &) const
{
    if(row > m_list.count())
        return QModelIndex();

    return QAbstractTableModel::createIndex(row, column, m_list[row]);
}

void TermModel::insert(TermItem* item)
{
    m_list.append(item);
}
