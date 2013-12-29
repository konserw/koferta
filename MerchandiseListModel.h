#ifndef TOWARMODEL_H
#define TOWARMODEL_H

#include <QAbstractTableModel>
#include <QHash>
#include <QList>

class Merchandise;
class QSqlTableModel;
class QSqlRecord;

/*!
 * \brief Model przechowywujący listę oferowanych towarów
 */
class MerchandiseListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MerchandiseListModel(QObject *parent = 0);
    virtual ~MerchandiseListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

    void setGlobalRabat(double r);

    void addItem(Merchandise* towar);
    void loadOffer(const QSqlTableModel& mod);

    bool pln() const;
    double kurs() const;

    bool isRabat(const QModelIndex & i) const;
    bool isIlosc(const QModelIndex & i) const;

    QHash<int, double> hash() const;

public slots:
    void clear();

    void setKurs(double kurs);

    ///dodawanie towarów do tabeli (wywoływane przez sygnał z dialogu dodajTowar)
    void changeItemCount(int id, double ile);

protected:
    QList<Merchandise*> m_list;

    bool m_pln;
    double m_kurs;

    double przeliczSume() const;
};

#endif // TOWARMODEL_H
