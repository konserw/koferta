#include "LoadDialogMerchandiseListModel.h"

LoadDialogMerchandiseListModel::LoadDialogMerchandiseListModel(QObject *parent) :
    QSqlTableModel(parent)
{
    setTable("savedOffersMerchandiseShortView");
    setEditStrategy(QSqlTableModel::OnManualSubmit);

    setHeaderData(0, Qt::Horizontal, tr("Kod"));
    setHeaderData(1, Qt::Horizontal, tr("Opis"));
    setHeaderData(2, Qt::Horizontal, tr("Ilość"));
    setHeaderData(3, Qt::Horizontal, tr("Rabat"));
}

void LoadDialogMerchandiseListModel::setOfferId(const QString &offerId)
{
    setFilter(QString("nr_oferty = '%1'").arg(offerId));
    select();
}
