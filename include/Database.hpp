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
#include "TermItem.hpp"
#include "TermModel.hpp"
#include "OfferSearchModel.hpp"
#include "User.hpp"

class QSqlDatabase;
class QString;
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
    void setPassword(int uid, QString password);
    int getNewOfferNumber(int uid) const;
    QString getNewOfferSymbolForUser(const User &u) const;

    //terms
    static void createTerm(const TermItem& term);
    static TermModel* getTermModel(TermType termType);
    static TermItem getTerm(TermType termType, int id = -1);

    //offer
    void saveOffer(const Offer& offer) const;
    void loadOffer(Offer *offer, int offerID);

    //models
    static LoadDialogMerchandiseListModel* loadDialogMerchandiseListModel(QObject* parent);
    static OfferSearchModel* offerSearchModel(QObject* parent);

    //other
    static QString mainAddress();

    bool isConnected() const;
public slots:
    void dropConection();
    void setupDatabaseConnection(const QString &host, unsigned port, const QString &schema, const QString &user = QString(), const QString &password = QString());
    User logIn(int uid, const QString& password);

    void deleteCustomer(const Customer &customer) const;
    void editCustomer(const Customer& customer) const;
    void saveCustomer(const Customer& customer) const;

signals:
    void connectionSuccess();
    void changeStatus(const QString&);

protected:
    explicit Database();
    ~Database();
    static Database* m_instance;
    QSqlDatabase* m_database;
};

#endif // DATABASE_H
