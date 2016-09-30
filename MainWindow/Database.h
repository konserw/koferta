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

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QProcess>
#include "TermModel.h"

class QString;
class User;
class QSqlTableModel;
class Merchandise;
class LoadDialogMerchandiseListModel;
class QProcess;
class QTcpSocket;
class QProgressDialog;
class Customer;
class Offer;

class Database : public QObject
{
Q_OBJECT

public:
    static Database* instance();

/* Database Interface */
    //user-related
    QHash<QString, int> usersList();
    bool setCurrentOfferNumber(int offerNumber);

    //terms
    static bool createTerm(const TermItem& term);
    static TermModel* getTermModel(TermItem::TermType termType);
    static TermItem getTerm(TermItem::TermType termType, int id = -1);

    static TermItem paymentTerm(int id);
    static TermItem shippingTerm(int id);
    static TermItem shipmentTime(int id);
    static TermItem offerTerm(int id);

    //merchandise todo - do wywalenia przynajmniej 2
    static QList<Merchandise*> loadOfferMerchandise(const QString& number);
    static LoadDialogMerchandiseListModel* loadDialogMerchandiseListModel(QObject* parent);

    //other
    static QString mainAddress();

    bool setPassword(int uid, QString password);
public slots:
    void dropConection();
    void setupDatabaseConnection(const QString &host, unsigned port, const QString &schema);
    bool logIn(int uid, const QString& password);

    bool save(const Customer& client) const;
    bool save(const Offer& offer) const;
    void loadOffer(Offer *offer, const QString &offerId);

    void changePasswordDialog();
signals:
    void connectionSuccess();
    void changeStatus(const QString&);

protected:
    explicit Database();
    ~Database();
    static Database* m_instance;
    QSqlDatabase* m_database;

    //SQL Tranaction/Query support
    static QSqlQuery transactionQuery(const QString& queryText);
    static bool transactionRun(const QString &queryText);
    static bool transactionOpen();
    static bool transactionClose();

    static QByteArray hmacSha1(QByteArray key, QByteArray baseString);
    static QString saltPassword(const QString &salt, const QString &password);
};

//TODO wywalic to !
#define EXEC_SILENT(s) \
do{ \
    if(q.exec(s) == false) \
    { \
        qDebug() << "Zapytanie mysql zkończone niepowodzeniem!"; \
        qDebug()  << "\tZapytanie mysql: " << s; \
        qDebug()  << "\tError text: " <<  q.lastError().text(); \
        QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
        return; \
    } \
}while(0)

#endif // DATABASE_H
