#include "MerchandiseListModel.h"
#include "Merchandise.h"
#include <QDebug>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlQuery>

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
    if(role == Qt::EditRole && isRabat(index))
    {
        Merchandise* t = static_cast<Merchandise*>(index.internalPointer());
        t->setRabat(value.toDouble());
        emit dataChanged(index, index);
        return true;
    }
    if(role == Qt::EditRole && isIlosc(index))
    {
        Merchandise* t = static_cast<Merchandise*>(index.internalPointer());
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
            return t->rabat(); //m_list[index.row()]->rabat();
        }
        if(isIlosc(index))
        {
            return t->ilosc(); //m_list[index.row()]->ilosc();
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
    if(!role == Qt::DisplayRole)
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

Qt::ItemFlags MerchandiseListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    if(isRabat(index) || isIlosc(index))
        return Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
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
    if(m_pln && (m_kurs == 0 || m_kurs == 1 || m_kurs == -1))
    {
        m_kurs = 1;
        beginRemoveColumns(QModelIndex(), 3, 3);
        m_pln = false;
        endRemoveColumns();
    }
    else
    {
        m_kurs = kurs;
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
        Merchandise* x = new Merchandise(id, r.value("code").toString(), r.value("desc").toString(), r.value("price").toDouble(), r.value("unit").toString() == "mb.");
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

QString MerchandiseListModel::print(const int w, bool kod, bool towar, bool ilosc, bool cenaKat, bool rabat, bool cena) const
{
    uint rows = m_list.count();

    QString waluta;
    if(pln()) waluta = "zł";
    else waluta= "€";

    //szerokosc komorek tabeli
    int kolumn = 3;
    int z[8];
    z[0] = 20; //lp
    z[1] = 80; //kod
    //z2 - reszta szerokości
    z[3] = 60; //cena
    z[4] = 40; //rabat
    z[5] = 55; //cena2
    z[6] = 50; //ilosc+jedn
    z[7] = 60; //wartość

    z[2] = w - z[0] - z[7];
    if(kod)
    {
        z[2] -= z[1];
        kolumn++;
    }
    if(cenaKat)
    {
        z[2] -= z[3];
        kolumn++;
    }
    if(rabat)
    {
        z[2] -= z[4];
        kolumn++;
    }
    if(cena)
    {
        z[2] -= z[5];
        kolumn++;
    }
    if(ilosc)
    {
        z[2] -= z[6];
        kolumn++;
    }
    z[2] -= kolumn*4;

    QString doc;
    doc = QString("\t<table cellspacing=3>\n"
            "\t<thead><tr>\n"
            "\t\t<td width=%1>LP</td>\n").arg(z[0]);
    if(kod)
        doc += QString("\t\t<td width=%1>Kod</td>\n").arg(z[1]);
    if(towar)
        doc += QString("\t\t<td width=%1>Specyfikacja</td>\n").arg(z[2]);
    if(ilosc)
        doc += QString("\t\t<td width=%1 align = right>Ilość</td>\n").arg(z[6]);
    if(cenaKat)
        doc += QString("\t\t<td width=%1 align = right>Cena kat. %2</td>\n").arg(z[3]).arg(waluta);
    if(rabat)
        doc += QString("\t\t<td width=%1 align = right>Rabat</td>\n").arg(z[4]);
    if(cena)
        doc += QString("\t\t<td width=%1 align = right>Cena %2</td>\n").arg(z[5]).arg(waluta);
    doc += QString("\t\t<td width=%1 align = right>Wartosc %2</td>\n").arg(z[7]).arg(waluta);
    doc += "\t</tr></thead>\n";

    for(uint i=0; i<rows; ++i)
    {
        Merchandise* item = m_list[i];
        doc += "\t<tr>\n";
        doc += QString("\t\t<td>%1</td>\n").arg(i+1);
        if(kod)
            doc += QString("\t\t<td>%1</td>\n").arg(item->kod());
        if(towar)
            doc += QString("\t\t<td>%1</td>\n").arg(item->nazwa());
        if(ilosc)
            doc += QString("\t\t<td align = right>%1 %2</td>\n").arg(item->ilosc()).arg(item->unit());
        if(cenaKat)
            doc += QString("\t\t<td align = right>%1</td>\n").arg(item->cenaKat());
        if(rabat)
            doc += QString("\t\t<td align = right>%1</td>\n").arg(item->rabat());
        if(cena)
            doc += QString("\t\t<td align = right>%1</td>\n").arg(item->cena());
        doc += QString("\t\t<td align = right>%1</td>\n").arg(item->wartosc());
        doc += "\t</tr>\n";
    }

    doc += "\t<tr>\n";
    doc += QString("\t\t<td align = right colspan=%1>Razem %2:</td>\n").arg(kolumn-1).arg(waluta);
    doc += QString("\t\t<td align = right>%1</td>\n").arg(przeliczSume());
    doc += "\t</tr>\n";
    doc += "\t</table>\n";

    return doc;
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
    qDebug() << "number" << number;
    /**************** TO DO ********************
     * wszystkie takie w klasie bazy danych
     * *********************************************/
    QSqlTableModel towary;
    towary.setTable("savedOffersMerchandiseView");
    towary.setFilter(QString("nr_oferty = '%1'").arg(number));
    towary.select();

    while(towary.canFetchMore())
        towary.fetchMore();

    Merchandise* t;
    QSqlRecord record;
    int rows = towary.rowCount();

    for(int row=0; row < rows; ++row)
    {
        record = towary.record(row);

        t = new Merchandise(record.value("merchandise_id").toInt(), record.value("code").toString(), record.value("desc").toString(), record.value("price").toDouble(), record.value("unit").toString() == "mb.");
        t->setRabat(record.value("rabat").toDouble());
        t->setIlosc(record.value("ilosc").toInt());

        addItem(t);
    }
}

