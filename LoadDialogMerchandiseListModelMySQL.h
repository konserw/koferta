#ifndef LOADDIALOGMERCHANDISELISTMODELMYSQL_H
#define LOADDIALOGMERCHANDISELISTMODELMYSQL_H

#include "LoadDialogMerchandiseListModel.h"

class LoadDialogMerchandiseListModelMySQL : public LoadDialogMerchandiseListModel
{
    Q_OBJECT

public:
    LoadDialogMerchandiseListModelMySQL(QObject* parent = 0);

public slots:
    virtual void setOfferId(const QString& offerId);
};

#endif // LOADDIALOGMERCHANDISELISTMODELMYSQL_H
