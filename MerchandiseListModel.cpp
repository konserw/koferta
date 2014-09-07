#include "MerchandiseListModel.h"
#include "Merchandise.h"
#include "Database.h"
#include <QDebug>
#include <QSqlTableModel>
#include <QtSql>
#include <QMessageBox>

MerchandiseListModel::MerchandiseListModel(QObject *parent) :
    QAbstractTableModel(parent), m_pln(false), m_kurs(1)
{
}

MerchandiseListModel::~MerchandiseListModel()
{
    qDeleteAll(m_list);
}

int MerchandiseListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_list.count() + 1;
}

int MerchandiseListModel::columnCount(const QModelIndex & /*parent*/) const
{
    return (m_pln ? 9 : 8);
}

bool MerchandiseListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role != Qt::EditRole || !index.isValid())
        return false;

    Merchandise* t = static_cast<Merchandise*>(index.internalPointer());

    if(isRabat(index))
    {
        t->setRabat(value.toDouble());
        emit dataChanged(index, index);
        return true;
    }
    else if(isIlosc(index))
    {
        t->setIlosc(value.toDouble());
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

QVariant MerchandiseListModel::data(const QModelIndex &index, int role) const
{

    if(!(role == Qt::DisplayRole || role == Qt::EditRole) || !index.isValid() || (index.row() > m_list.count()))
        return QVariant();

    Merchandise* t = static_cast<Merchandise*>(index.internalPointer());
    if(t == nullptr && index.row() < m_list.count())
    {
        qWarning() << "NULL internal pointer";
        return QVariant();
    }

    if(role == Qt::EditRole)
    {
        if(isRabat(index))
        {
            return t->rabat();
        }
        if(isIlosc(index))
        {
            return t->ilosc();
        }
        return QVariant();
    }

    if(m_pln)
    {
        if(index.row() == m_list.count())
        {
            if(index.column() == 7)
                return tr("Suma:");
            if(index.column() == 8)
                return QString::number(przeliczSume(), 'f', 2);
            return QVariant();
        }
        else switch(index.column())
        {
            case 0: return t->kod();
            case 1: return t->nazwa();
            case 2: return QString::number(t->cenaKat(), 'f', 2);
            case 3: return QString::number(t->cenaPln(m_kurs), 'f', 2);
            case 4: return QString::number(t->rabat(), 'f', 2);
            case 5: return QString::number(t->cena(m_kurs), 'f', 2);
            case 6: return QString::number(t->ilosc(), 'f', 2);
            case 7: return tr(t->metr() ? "mb." : "szt.");
            case 8: return QString::number(t->wartosc(m_kurs), 'f', 2);
        }
    }
    else
    {
        if(index.row() == m_list.count())
        {
            if(index.column() == 6)
                return tr("Suma:");
            if(index.column() == 7)
                return QString::number(przeliczSume(), 'f', 2);
            return QVariant();
        }
        else switch(index.column())
        {
            case 0: return t->kod();
            case 1: return t->nazwa();
            case 2: return QString::number(t->cenaKat(), 'f', 2);
            case 3: return QString::number(t->rabat(), 'f', 2);
            case 4: return QString::number(t->cena(), 'f', 2);
            case 5: return QString::number(t->ilosc(), 'f', 2);
            case 6: return tr(t->metr() ? "mb." : "szt.");
            case 7: return QString::number(t->wartosc(), 'f', 2);
        }
    }
    qWarning() << "View requested non-existant column, number:" << index.column();

    return QVariant();
}

QVariant MerchandiseListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Vertical)
    {
        if(section == m_list.count())
            return QVariant();

        return QString::number(section+1);
    }

    if(m_pln) switch(section)
    {
    case 0: return tr("Kod");
    case 1: return tr("Nazwa");
    case 2: return tr("Cena Kat. €");
    case 3: return tr("Cena Kat. zł");
    case 4: return tr("Rabat");
    case 5: return tr("Cena zł");
    case 6: return tr("Ilość");
    case 7: return tr("j/m");
    case 8: return tr("Wartość zł");
    }
    else switch(section)
    {
    case 0: return tr("Kod");
    case 1: return tr("Nazwa");
    case 2: return tr("Cena Kat. €");
    case 3: return tr("Rabat");
    case 4: return tr("Cena €");
    case 5: return tr("Ilość");
    case 6: return tr("j/m");
    case 7: return tr("Wartość €");
    }

    qWarning() << "View requested non-existant column, number:" << section;
    return QVariant();
}

Qt::DropActions MerchandiseListModel::supportedDropActions() const
{
     return Qt::MoveAction;
}

Qt::ItemFlags MerchandiseListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    if(index.row() == m_list.count())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    Qt::ItemFlags def = Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if(isRabat(index) || isIlosc(index))
        return Qt::ItemIsEditable | def;

    return def;
}

QModelIndex MerchandiseListModel::index(int row, int column, const QModelIndex & /*parent*/) const
{
    if(row > m_list.count())
        return QModelIndex();
    else if(row == m_list.count())
        return QAbstractTableModel::createIndex(row, column, nullptr);
    return QAbstractTableModel::createIndex(row, column, m_list[row]);
}

bool MerchandiseListModel::pln() const
{
    return m_pln;
}

void MerchandiseListModel::clear()
{
    if(m_list.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, m_list.count()-1);
    qDeleteAll(m_list);
    m_list.clear();
    endRemoveRows();
}
double MerchandiseListModel::kurs() const
{
    return m_kurs;
}

bool MerchandiseListModel::isRabat(const QModelIndex &i) const
{
    if(i.row() < m_list.count() && ((m_pln && i.column() == 4) || (!m_pln && i.column() == 3)))
        return true;
    return false;
}

bool MerchandiseListModel::isIlosc(const QModelIndex &i) const
{
    if(i.row() < m_list.count() && ((m_pln && i.column() == 6) || (!m_pln && i.column() == 5)))
        return true;
    return false;
}

void MerchandiseListModel::setKurs(double kurs)
{
    m_kurs = kurs;

    if(m_pln && kurs == -1)
    {
        m_kurs = 1;
        beginRemoveColumns(QModelIndex(), 3, 3);
        m_pln = false;
        endRemoveColumns();
    }
    else if(!m_pln)
    {   
        beginInsertColumns(QModelIndex(), 3, 3);
        m_pln = true;
        endInsertColumns();
    }
}

void MerchandiseListModel::changeItemCount(int id, double ile)
{
    Merchandise* t = new Merchandise(id);
    if(m_list.contains(t))
    {
        if(ile > 0)
        {
            Merchandise* x = m_list[m_list.indexOf(t)];
            x->setIlosc(ile);
        }
        else
            removeRow(m_list.indexOf(t));

    }
    else if(ile > 0)
    {
        /********************************* TO DO **********************
            databse object should handle retriving data from db
            *************************************************************/
        QSqlQuery r;
        r.exec(QString("select * from merchandise where id = %1").arg(id));
        r.next();
        Merchandise* x = new Merchandise(id, r.value("code").toString(), r.value("description").toString(), r.value("price").toDouble(), r.value("unit").toString() == "mb.");
        x->setIlosc(ile);
        beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
        m_list.append(x);
        endInsertRows();
    }
    delete t;
}

bool MerchandiseListModel::removeRows(int row, int count, const QModelIndex & /*parent*/)
{
    if(row == m_list.count())
        return false;

    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=row; i < row+count; i++)
    {
        delete m_list[i];
        m_list.removeAt(i);
    }
    endRemoveRows();

    return true;
}

bool MerchandiseListModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    int sourceFirst = sourceRow;
    int sourceLast = sourceRow + count - 1;

    if(destinationChild >= sourceFirst && destinationChild <= sourceLast)
        return false;

    if(destinationChild == sourceLast+1)
        return false;

    beginMoveRows(sourceParent, sourceFirst, sourceLast, destinationParent, destinationChild);

    int offset = destinationChild - sourceRow;
    if(destinationChild == m_list.length()) offset--;
    for(int i = 0; i < count; ++i)
        m_list.move(sourceRow + i, sourceRow + offset + i);


    endMoveRows();
    return true;
}

void MerchandiseListModel::setGlobalRabat(double r)
{
    foreach(Merchandise* t, m_list)
    {
        t->setRabat(r);
    }
// potrzebne?   emit dataChanged(this->index(2, 0), this->index(m_list.count(), 8)); -- ?
}

QHash<int, double> MerchandiseListModel::hash() const
{
    QHash<int, double> hash;

    foreach(Merchandise* t, m_list)
        hash.insert(t->id(), t->ilosc());

    return hash;
}

QString MerchandiseListModel::print(const int w, bool ilosc, bool cenaKat, bool cenaPln, bool rabat, bool cena, bool specyfikacja) const
{
    QString waluta;
    if(pln()) waluta = "zł";
    else waluta= "€";

    const int columnWidthOrderNumber = 40;
    const int columnWidthPrice = 90;
    const int columnWidthNarrow = 70; //discount, quontity

    int columnWidthMerchandise = w - columnWidthOrderNumber - columnWidthPrice;
    int cols = 3;

    if(cenaKat)
    {
        columnWidthMerchandise -= columnWidthPrice;
        cols++;
    }
    if(cenaPln)
    {
        columnWidthMerchandise -= columnWidthPrice;
        cols++;
    }
    if(rabat)
    {
        columnWidthMerchandise -= columnWidthNarrow;
        cols++;
    }
    if(cena)
    {
        columnWidthMerchandise -= columnWidthPrice;
        cols++;
    }
    if(ilosc)
    {
        columnWidthMerchandise -= columnWidthNarrow;
        cols++;
    }
    columnWidthMerchandise -= cols*4;

    QString doc;
    doc = QString("\t<table cellspacing=2>\n"
            "\t<thead><tr>\n"
            "\t\t<td width=%1><b>%2</b></td>\n").arg(columnWidthOrderNumber).arg("Lp.");

    doc += QString("\t\t<td width=%1><b>%2</b></td>\n").arg(columnWidthMerchandise).arg("Towar");
    if(cenaKat)
        doc += QString("\t\t<td width=%1 align = right><b>%2</b></td>\n").arg(columnWidthPrice).arg("Cena kat. €");
    if(cenaPln)
        doc += QString("\t\t<td width=%1 align = right><b>%2</b></td>\n").arg(columnWidthPrice).arg("Cena kat. zł");
    if(rabat)
        doc += QString("\t\t<td width=%1 align = right><b>%2</b></td>\n").arg(columnWidthNarrow).arg("Rabat");
    if(cena)
        doc += QString("\t\t<td width=%1 align = right><b>%3 %2</b></td>\n").arg(columnWidthPrice).arg(waluta).arg("Cena");
    if(ilosc)
        doc += QString("\t\t<td width=%1 align = right><b>%2</b></td>\n").arg(columnWidthNarrow).arg("Ilość");
    doc += QString("\t\t<td width=%1 align = right><b>%3 %2</b></td>\n").arg(columnWidthPrice).arg(waluta).arg("Wartość");
    doc += "\t</tr></thead>\n";

    uint rows = m_list.count();
    for(uint i=0; i<rows; ++i)
    {
        Merchandise* item = m_list[i];
        double dCena;
        double wartosc;
        if(m_pln)
        {
            dCena = item->cena(m_kurs);
            wartosc = item->wartosc(m_kurs);
        }
        else
        {
            dCena = item->cena();
            wartosc = item->wartosc(m_kurs);
        }

        doc += "\t<tr>\n";
        doc += QString("\t\t<td>%1</td>\n").arg(i+1);
        doc += QString("\t\t<td>%1</td>\n").arg(item->kod());
        if(cenaKat)
            doc += QString("\t\t<td align = right>%1</td>\n").arg(item->cenaKat(), 0, 'f', 2);
        if(cenaPln)
            doc += QString("\t\t<td align = right>%1</td>\n").arg(item->cenaPln(m_kurs), 0, 'f', 2);
        if(rabat)
            doc += QString("\t\t<td align = right>%1%</td>\n").arg(item->rabat());
        if(cena)
            doc += QString("\t\t<td align = right>%1</td>\n").arg(dCena, 0, 'f', 2);
        if(ilosc)
            doc += QString("\t\t<td align = right>%1 %2</td>\n").arg(item->ilosc(), 0, 'f', 0).arg(item->unit());
        doc += QString("\t\t<td align = right>%1</td>\n").arg(wartosc, 0, 'f', 2);
        doc += "\t</tr>\n";
        if(specyfikacja)
        {
            doc += "\t<tr style=\"font-size:12px; font-family:\"Times New Roman\",Georgia,Serif;\">\n";
            doc += "\t\t<td></td>\n";
            doc += QString("\t\t<td colspan=%1>%2</td>\n").arg(cols-1).arg(item->nazwa());
            doc += "\t</tr>\n";
        }
    }

    doc += "\t<tr style=\"font-weight:bold;\">\n";
    doc += QString("\t\t<td align = right colspan=%1>Razem %2:</td>\n").arg(cols-1).arg(waluta);
    doc += QString("\t\t<td align = right>%1</td>\n").arg(przeliczSume(), 0, 'f', 2);
    doc += "\t</tr>\n";
    doc += "\t</table>\n";

    return doc;
}

void MerchandiseListModel::save(const QString &offerId)
{
    Database::saveOfferMerchandise(offerId, m_list);
}

double MerchandiseListModel::przeliczSume() const
{
    double suma = 0;
    foreach(Merchandise* t, m_list)
        suma += t->wartosc(m_kurs);
    return suma;
}

void MerchandiseListModel::addItem(Merchandise *towar)
{
    int numer = m_list.count();
    beginInsertRows(QModelIndex(), numer, numer);
    m_list.append(towar);
    endInsertRows();
}

void MerchandiseListModel::loadOffer(const QString& number)
{
    clear();

    QList<Merchandise *> list = Database::loadOfferMerchandise(number);

    if(list.count() > 0)
    {
        beginInsertRows(QModelIndex(), 0, list.count());
        m_list.append(list);
        endInsertRows();
    }
}
