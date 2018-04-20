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
#include "TermItem.h"
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
    void setPassword(int uid, QString password);
    int getNewOfferNumber();

    //terms
    static void createTerm(const TermItem& term);
    static TermModel* getTermModel(TermType termType);
    static TermItem getTerm(TermType termType, int id = -1);

    static LoadDialogMerchandiseListModel* loadDialogMerchandiseListModel(QObject* parent);

    //other
    static QString mainAddress();

public slots:
    void dropConection();
    void setupDatabaseConnection(const QString &host, unsigned port, const QString &schema);
    bool logIn(int uid, const QString& password);

    void deleteCustomer(Customer c) const;
    void editCustomer(const Customer& customer) const;
    void saveCustomer(const Customer& customer) const;

    void saveOffer(const Offer& offer) const;
    void loadOffer(Offer *offer, int offerID);

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
