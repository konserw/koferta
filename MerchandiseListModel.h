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

    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild);

    void addItem(Merchandise* towar);
    void loadOffer(const QString &number);

    bool pln() const;
    double kurs() const;
    void setGlobalRabat(double r);

    bool isRabat(const QModelIndex & i) const;
    bool isIlosc(const QModelIndex & i) const;

    QHash<int, double> hash() const;
    QString print(const int w, bool ilosc, bool cenaKat, bool cenaPln, bool rabat, bool cena, bool specyfikacja) const;

    void save(const QString& offerId);

public slots:
    void clear();
    void setKurs(double kurs);
    void changeItemCount(int id, double ile);

protected:
    QList<Merchandise*> m_list;

    bool m_pln;
    double m_kurs;

    double przeliczSume() const;
};

#endif // TOWARMODEL_H
