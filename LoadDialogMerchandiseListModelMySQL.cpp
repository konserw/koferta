#include "LoadDialogMerchandiseListModelMySQL.h"

LoadDialogMerchandiseListModelMySQL::LoadDialogMerchandiseListModelMySQL(QObject *parent) :
    LoadDialogMerchandiseListModel(parent)
{
    setTable("savedOffersMerchandiseShortView");
}

void LoadDialogMerchandiseListModelMySQL::setOfferId(const QString &offerId)
{
    setFilter(QString("nr_oferty = '%1'").arg(offerId));
    select();
}
