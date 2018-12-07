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

#include "MerchandiseListModel.hpp"
#include "Merchandise.hpp"
#include "Database.hpp"

#include <QDebug>
#include <QSqlTableModel>
#include <QtSql>
#include <QMessageBox>
#include <algorithm>

MerchandiseListModel::MerchandiseListModel(QObject *parent) :
    QAbstractTableModel(parent), currencyIsPLN(false), exchangeRate(1)
{
}

MerchandiseListModel::MerchandiseListModel(QList<Merchandise *> list, QObject *parent) :
    MerchandiseListModel(parent)
{
    m_list = list;
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
    return (currencyIsPLN ? 9 : 8);
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
    if (role == Qt::TextAlignmentRole) {
        if (index.column() > 1) {
            return Qt::AlignRight;
        } else {
            return Qt::AlignLeft;
        }
    }

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

    if(currencyIsPLN)
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
            case 3: return QString::number(t->cenaKat(exchangeRate), 'f', 2);
            case 4: return QString::number(t->rabat(), 'f', 2);
            case 5: return QString::number(t->cena(exchangeRate), 'f', 2);
            case 6: return QString::number(t->ilosc(), 'f', 2);
            case 7: return tr(t->metr() ? "mb." : "szt.");
            case 8: return QString::number(t->wartosc(exchangeRate), 'f', 2);
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

    if(currencyIsPLN) switch(section)
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

bool MerchandiseListModel::isPLN() const
{
    return currencyIsPLN;
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
double MerchandiseListModel::getExchangeRate() const
{
    return exchangeRate;
}

bool MerchandiseListModel::isRabat(const QModelIndex &i) const
{
    if(i.row() < m_list.count() && ((currencyIsPLN && i.column() == 4) || (!currencyIsPLN && i.column() == 3)))
        return true;
    return false;
}

bool MerchandiseListModel::isIlosc(const QModelIndex &i) const
{
    if(i.row() < m_list.count() && ((currencyIsPLN && i.column() == 6) || (!currencyIsPLN && i.column() == 5)))
        return true;
    return false;
}

void MerchandiseListModel::setExchangeRate(double kurs)
{
    exchangeRate = kurs;
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

void MerchandiseListModel::setPLN(bool value)
{
    if(currencyIsPLN == true && value == false) //back to EUR
    {
        beginRemoveColumns(QModelIndex(), 3, 3);
        exchangeRate = 1;
        currencyIsPLN = false;
        endRemoveColumns();
    }
    else if(currencyIsPLN == false && value == true)
    {
        beginInsertColumns(QModelIndex(), 3, 3);
        currencyIsPLN = true;
        endInsertColumns();
    }
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

void MerchandiseListModel::sort(int column, Qt::SortOrder order)
{
    auto begin = m_list.begin();
    auto end = m_list.end();
    double kurs = exchangeRate;

    beginResetModel();

    if(column == 0)//kod
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->kod() < b->kod(); });
        else
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->kod() > b->kod(); });
    }
    else if(column == 1)//nazwa
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->nazwa() < b->nazwa(); });
        else
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->nazwa() > b->nazwa(); });
    }
    else if(column == 2)//Cena Kat. €
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->cenaKat() < b->cenaKat(); });
        else
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->cenaKat() > b->cenaKat(); });
    }
    else if(column == 3 && currencyIsPLN)//Cena Kat. zl
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [&kurs](Merchandise* a, Merchandise* b) { return a->cenaKat(kurs) < b->cenaKat(kurs); });
        else
            std::sort(begin, end, [&kurs](Merchandise* a, Merchandise* b) { return a->cenaKat(kurs) > b->cenaKat(kurs); });
    }
    else if(column == 3 || (column == 4 && currencyIsPLN))//Rabat
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->rabat() < b->rabat(); });
        else
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->rabat() > b->rabat(); });
    }
    else if(column == 4)//Cena euro
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->cena() < b->cena(); });
        else
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->cena() > b->cena(); });
    }
    else if(column == 5 && currencyIsPLN)//Cena zl
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [&kurs](Merchandise* a, Merchandise* b) { return a->cena(kurs) < b->cena(kurs); });
        else
            std::sort(begin, end, [&kurs](Merchandise* a, Merchandise* b) { return a->cena(kurs) > b->cena(kurs); });
    }
    else if(column == 5 || (column == 6 && currencyIsPLN))//Ilosc
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->ilosc() < b->ilosc(); });
        else
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->ilosc() > b->ilosc(); });
    }
    else if(column == 6 || (column == 7 && currencyIsPLN))//jednostka
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [](Merchandise* a, Merchandise* /*b*/) { return a->metr(); });
        else
            std::sort(begin, end, [](Merchandise* /*a*/, Merchandise* b) { return b->metr(); });
    }
    else if(column == 7)//wartosc
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->wartosc() < b->wartosc(); });
        else
            std::sort(begin, end, [](Merchandise* a, Merchandise* b) { return a->wartosc() > b->wartosc(); });
    }
    else if(column == 8)//wartosc zl
    {
        if(order == Qt::AscendingOrder)
            std::sort(begin, end, [&kurs](Merchandise* a, Merchandise* b) { return a->wartosc(kurs) < b->wartosc(kurs); });
        else
            std::sort(begin, end, [&kurs](Merchandise* a, Merchandise* b) { return a->wartosc(kurs) > b->wartosc(kurs); });
    }

    endResetModel();
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

QString MerchandiseListModel::print(const int w, Offer::PrintOptions printOptions) const
{
    QString waluta;
    if(isPLN()) waluta = "zł";
    else waluta= "€";

    const int columnWidthOrderNumber = 40;
    const int columnWidthPrice = 90;
    const int columnWidthNarrow = 70; //discount, quontity

    int columnWidthMerchandise = w - columnWidthOrderNumber - columnWidthPrice;
    int cols = 3;

    if(printOptions.testFlag(Offer::printRawPrice))//cenaKat)
    {
        columnWidthMerchandise -= columnWidthPrice;
        cols++;
    }
    if(printOptions.testFlag(Offer::printRawPricePLN))//cenaPln)
    {
        columnWidthMerchandise -= columnWidthPrice;
        cols++;
    }
    if(printOptions.testFlag(Offer::printDiscount))//rabat)
    {
        columnWidthMerchandise -= columnWidthNarrow;
        cols++;
    }
    if(printOptions.testFlag(Offer::printPrice))//cena)
    {
        columnWidthMerchandise -= columnWidthPrice;
        cols++;
    }
    if(printOptions.testFlag(Offer::printNumber))//ilosc)
    {
        columnWidthMerchandise -= columnWidthNarrow;
        cols++;
    }
    columnWidthMerchandise -= cols*4;

    QString doc;
    doc = QString("\t<table cellspacing=0>\n"
            "\t<thead><tr class=\"header\">\n"
            "\t\t<td width=%1><b>%2</b></td>\n").arg(columnWidthOrderNumber).arg("Lp.");

    doc += QString("\t\t<td width=%1><b>%2</b></td>\n").arg(columnWidthMerchandise).arg("Towar");
    if(printOptions.testFlag(Offer::printRawPrice))
        doc += QString("\t\t<td width=%1 align = right><b>%2</b></td>\n").arg(columnWidthPrice).arg("Cena kat. €");
    if(printOptions.testFlag(Offer::printRawPricePLN))
        doc += QString("\t\t<td width=%1 align = right><b>%2</b></td>\n").arg(columnWidthPrice).arg("Cena kat. zł");
    if(printOptions.testFlag(Offer::printDiscount))
        doc += QString("\t\t<td width=%1 align = right><b>%2</b></td>\n").arg(columnWidthNarrow).arg("Rabat");
    if(printOptions.testFlag(Offer::printPrice))
        doc += QString("\t\t<td width=%1 align = right><b>%3 %2</b></td>\n").arg(columnWidthPrice).arg(waluta).arg("Cena");
    if(printOptions.testFlag(Offer::printNumber))
        doc += QString("\t\t<td width=%1 align = right><b>%2</b></td>\n").arg(columnWidthNarrow).arg("Ilość");
    doc += QString("\t\t<td width=%1 align = right><b>%3 %2</b></td>\n").arg(columnWidthPrice).arg(waluta).arg("Wartość");
    doc += "\t</tr></thead>\n";

    auto rows = m_list.count();
    for(auto i=0; i<rows; ++i)
    {
        Merchandise* item = m_list[i];
        double dCena;
        double wartosc;
        if(currencyIsPLN)
        {
            dCena = item->cena(exchangeRate);
            wartosc = item->wartosc(exchangeRate);
        }
        else
        {
            dCena = item->cena();
            wartosc = item->wartosc(exchangeRate);
        }
        QString style = i%2 == 0 ? "row0" : "row1";

        doc += QString("\t<tr class=\"%1\">\n").arg(style);
        doc += QString("\t\t<td>%1</td>\n").arg(i+1);
        doc += QString("\t\t<td>%1</td>\n").arg(item->kod());
        if(printOptions.testFlag(Offer::printRawPrice))
            doc += QString("\t\t<td align = right>%1</td>\n").arg(item->cenaKat(), 0, 'f', 2);
        if(printOptions.testFlag(Offer::printRawPricePLN))
            doc += QString("\t\t<td align = right>%1</td>\n").arg(item->cenaKat(exchangeRate), 0, 'f', 2);
        if(printOptions.testFlag(Offer::printDiscount))
            doc += QString("\t\t<td align = right>%1%</td>\n").arg(item->rabat());
        if(printOptions.testFlag(Offer::printPrice))
            doc += QString("\t\t<td align = right>%1</td>\n").arg(dCena, 0, 'f', 2);
        if(printOptions.testFlag(Offer::printNumber))
            doc += QString("\t\t<td align = right>%1 %2</td>\n").arg(item->ilosc(), 0, 'f', 0).arg(item->unit());
        doc += QString("\t\t<td align = right>%1</td>\n").arg(wartosc, 0, 'f', 2);
        doc += "\t</tr>\n";
        if(printOptions.testFlag(Offer::printSpecs))
        {
            doc += QString("\t<tr class=\"%1 spec\">\n").arg(style);
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

VariantLists MerchandiseListModel::asVariantLists() const
{
    VariantLists lists;
    Merchandise* merchandise;
    for(int i=0; i < m_list.length(); ++i)
    {
        merchandise = m_list[i];
        lists.seq << i;
        lists.id << merchandise->id();
        lists.count << merchandise->ilosc();
        lists.disc << merchandise->rabat();
    }
    return lists;
}

double MerchandiseListModel::przeliczSume() const
{
    double suma = 0;
    foreach(Merchandise* t, m_list)
        suma += t->wartosc(exchangeRate);
    return suma;
}

void MerchandiseListModel::addItem(Merchandise *towar)
{
    int numer = m_list.count();
    beginInsertRows(QModelIndex(), numer, numer);
    m_list.append(towar);
    endInsertRows();
}
