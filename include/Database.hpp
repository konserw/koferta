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
#include <QString>
#include <QSqlRecord>

#include "TermItem.hpp"
#include "TermModel.hpp"

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

namespace Database
{
    //connection
    bool isConnected();
    void dropConection();
    void setupDatabaseConnection(const QString &host, unsigned port, const QString &schema, const QString &user = QString(), const QString &password = QString());

    //user-related
    QSqlRecord getUserData(int uid, const QString& password);
    QHash<QString, int> usersList();
    void setPassword(int uid, QString password);
    int getNewOfferNumber(int uid);

    //terms
    void createTerm(const TermItem& term);
    TermModel* getTermModel(TermType termType);
    TermItem getTerm(TermType termType, int id = -1);

    //offer
    void saveOffer(const Offer& offer);
    void loadOffer(Offer *offer, int offerID);

    //customer
    QSqlRecord getCustomerData(int id);
    void deleteCustomer(const Customer &customer);
    void editCustomer(const Customer& customer);
    void saveCustomer(const Customer& customer);

    //other
    QString mainAddress();
}

#endif // DATABASE_H
