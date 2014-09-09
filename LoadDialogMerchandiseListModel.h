#ifndef LOADDIALOGMERCHANDISELISTMODEL_H
#define LOADDIALOGMERCHANDISELISTMODEL_H

#include <QSqlTableModel>

class LoadDialogMerchandiseListModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit LoadDialogMerchandiseListModel(QObject *parent = 0);

public slots:
    void setOfferId(const QString& offerId);
};

#endif // LOADDIALOGMERCHANDISELISTMODEL_H
