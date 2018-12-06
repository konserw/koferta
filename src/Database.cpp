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
#include <random>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QVariant>
#include <QTcpSocket>
#include <QHash>

#include "Customer.hpp"
#include "Merchandise.hpp"
#include "Database.hpp"
#include "DatabaseHelpers.hpp"
#include "LoadDialogMerchandiseListModel.hpp"
#include "Offer.hpp"
#include "MerchandiseListModel.hpp"

static QHash<TermType, QString> termTable
{
    {TermType::billing, "billingTerms"},
    {TermType::delivery, "deliveryTerms"},
    {TermType::deliveryDate, "deliveryDateTerms"},
    {TermType::offer, "offerTerms"}
};

void Database::setupDatabaseConnection(const QString &host, quint16 port, const QString& schema, const QString& user, const QString& password)
{
    qDebug() << "Setup database connection";
    if(!QSqlDatabase::drivers().contains("QMYSQL"))
    {
        qCritical() << "No QMYSQL Driver";
        qDebug() << "library paths: ";
        QStringList list = qApp->libraryPaths();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];

        qDebug() << "aviable drivers: ";
        list = QSqlDatabase::drivers();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];
        throw DatabaseException("Failed to create database object - QMYSQL driver not found.",
                                QObject::tr("Nie odnaleziono sterownika bazy danych. Skontaktuj się z administratorem"));
    }

    qDebug().noquote().nospace() << "Check if TCP socket " << host << ":" << port << " is reachable";
    QTcpSocket socket;
    socket.connectToHost(host, port);
    if(socket.waitForReadyRead() == false)
    {
        throw DatabaseException("Socket connection failed",
                                QObject::tr("Wystąpił błąd połączenia - uruchom ponownie lub popraw dane połączenia"));
    }
    socket.disconnectFromHost();
    qDebug() << "Socket has been successfully opened";

    auto database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName(host);
    database.setPort(port);
    database.setDatabaseName(schema);
    if(!user.isNull() && !password.isNull())
    {
        database.setUserName(user);
        database.setPassword(password);
    }

    qDebug().nospace().noquote() << "Set up connection details:\n"
             << "\t* Host:\t\t" << database.hostName() << "\n"
             << "\t* Port:\t\t" << database.port() << "\n"
             << "\t* UserName:\t" << database.userName() << "\n"
//             << "\t* Password:\t" << QCryptographicHash::hash(database.password().toUtf8(), QCryptographicHash::Sha1).toBase64() << "\n"
             << "\t* Schema:\t" << database.databaseName();

    if(!database.open())
    {
        qCritical().nospace().noquote() << "Unable to connect to database! Details:\n"
        << "\t* connName:" << database.connectionName() << "\n"
        << "\t* driver:" << database.driverName() << "\n"
        << "\t* options:" << database.connectOptions() << "\n"
        << "\t* host:" << database.hostName() << "\n"
        << "\t* error number:" << database.lastError().number() << "\n"
        << "\t* database error:" << database.lastError().databaseText() << "\n"
        << "\t* driver error:" << database.lastError().driverText();

        throw DatabaseException("Database Connection failed",
                                QObject::tr("Połączenie z bazą danych nie powiodło się."));
    }
    if(QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions) == false)
    {
        qCritical().noquote() << "Database hasn't got transactions feature - disabling transactions";
        Transaction::disableTransactionFeature();
    }
}

void Database::setPassword(int uid, QString password)
{
    std::random_device rd;
    QString salt = QCryptographicHash::hash(QByteArray::number(rd()), QCryptographicHash::Sha1).toBase64();
    qDebug() << "new salt for user" << uid << "is" << salt;

    QString queryText = QString("UPDATE users SET password='%1', salt='%2', resetPassword=0 WHERE id=%3")
            .arg(saltPassword(salt, password))
            .arg(salt)
            .arg(uid);

    Transaction::open();
    Transaction::run(queryText);
    Transaction::commit();
}

QSqlRecord Database::getUserData(int uid, const QString &password)
{
    qDebug() << "Downloading user info for" << uid;

    QSqlTableModel usersTable;
    usersTable.setTable("users");
    usersTable.setFilter(QString("id = %1").arg(uid));
    usersTable.select();
    if(usersTable.rowCount() < 1)
        throw DatabaseException("Invalid user selected");

    QSqlRecord r = usersTable.record(0);
    //qDebug() << "user record" << r;
    QString dbPassword = r.value("password").toString();
    QString salt = r.value("salt").toString();

    qDebug() << "User's salt:" << salt;
    if(saltPassword(salt, password) != dbPassword)
        throw DatabaseException("Wrong password has been given", QObject::tr("Logowanie nie powiodło się; sprawdź użytkownika i hasło."));

    return r;
}

void Database::saveOffer(const Offer &offer, const User& user)
{
    qDebug().noquote() << "Saving offer symbol:" << offer.getSymbol();
    QString queryText;

    QString escapedRemarks = offer.getRemarks();
    escapedRemarks.replace("\'", "\\\'");
    queryText = QString("INSERT INTO savedOffers ("
                        "offerSymbol,"
                        "userID,"
                        "customerID,"
                        "offerDate,"
                        "inquiryDate,"
                        "InquiryNumber,"
                        "deliveryTerms,"
                        "deliveryDateTerms,"
                        "billingTerms,"
                        "offerTerms,"
                        "remarks,"
                        "dExchangeRate,"
                        "bPrintSpecs,"
                        "bPrintRawPrice,"
                        "bPrintRawPricePLN,"
                        "bPrintDiscount,"
                        "bPrintPrice,"
                        "bPrintNumber"
                        ") VALUES ('%2', %3, %4, '%5', %6, %7, %8, %9, %10, %11, '%12', %13, %14, %15, %16, %17, %18, %19)")
/* offerSymbol */   	.arg(offer.getSymbol())
/* userID */        	.arg(user.getUid())
/* customerID */    	.arg(offer.getCustomer().getIDorNull())
/* offerDate */     	.arg(offer.getDate().toString("yyyy-MM-dd"))
/* inquiryDate */   	.arg(offer.getInquiryDateSql())
/* InquiryNumber */ 	.arg(offer.getInquiryNumberSql())
/* deliveryTerms */ 	.arg(offer.getTermIDforDB(TermType::delivery))
/* deliveryDateTerms */	.arg(offer.getTermIDforDB(TermType::deliveryDate))
/* billingTerms */      .arg(offer.getTermIDforDB(TermType::billing))
/* offerTerms */        .arg(offer.getTermIDforDB(TermType::offer))
/* remarks */           .arg(escapedRemarks)
/* dExchangeRate */    	.arg(offer.getExchangeRateSql())
/* bPrintSpecs */       .arg(offer.getPrintOptionForDB(Offer::printSpecs))
/* bPrintRawPrice */    .arg(offer.getPrintOptionForDB(Offer::printRawPrice))
/* bPrintRawPricePLN */ .arg(offer.getPrintOptionForDB(Offer::printRawPricePLN))
/* bPrintDiscount */  	.arg(offer.getPrintOptionForDB(Offer::printDiscount))
/* bPrintPrice */       .arg(offer.getPrintOptionForDB(Offer::printPrice))
/* bPrintNumber */		.arg(offer.getPrintOptionForDB(Offer::printNumber));

    //save merchandise
    QSqlQuery query;
    query.prepare("INSERT INTO `savedOffersMerchandise` (`offerID`, `sequenceNumber`, `merchandiseID`, `quantity`, `discount`) "
                  "VALUES (LAST_INSERT_ID(), :seq, :mer, :ile, :rabat)");

    auto lists = offer.getMerchandiseAsVariantLists();
    query.addBindValue(lists.seq);
    query.addBindValue(lists.id);
    query.addBindValue(lists.count);
    query.addBindValue(lists.disc);

    Transaction::open();
    Transaction::run(queryText);
    Transaction::runBatch(query);
    Transaction::commit();
}

QSqlRecord Database::loadOfferBasic(int offerID)
{
    qDebug() << "Loading basic data for offer" << offerID;
    QSqlTableModel model;
    model.setTable("savedOffersFullView");
    model.setFilter(QString("offerID = '%1'").arg(offerID));
    if(model.select() == false)
        throw DatabaseException("Something went wrong while reading data from savedOffersFullView");
    return model.record(0);
}

QList<Merchandise *> Database::loadOfferMerchandise(int offerID)
{
    qDebug() << "Loading merchandise list for offer" << offerID;
    QString queryText = QString(
                        "SELECT * "
                        "FROM savedOffersMerchandiseView "
                        "WHERE offerID = %1 "
                        "ORDER BY sequenceNumber ASC"
                        ).arg(offerID);
    QSqlQuery query = Transaction::run(queryText);
    if(query.isActive() == false)
    {
        throw DatabaseException("Something went wrong when retriving merchandise list");
    }

    QList<Merchandise*> list;
    Merchandise* merchandise;
    while(query.next())
    {
        //qDebug() << "Loading merchandise: " << query.value("merchandiseID").toString();
        merchandise = new Merchandise(
                    query.value("merchandiseID").toInt(),
                    query.value("code").toString(),
                    query.value("description").toString(),
                    query.value("price").toDouble(),
                    query.value("unit").toString() == "mb.");
        merchandise->setRabat(query.value("discount").toDouble());
        merchandise->setIlosc(query.value("quantity").toInt());
        list.append(merchandise);
    }
    return list;
}

void Database::deleteCustomer(const Customer& customer)
{
    qDebug().noquote() <<  QString("deleting customer >%1< from database").arg(customer.concatedName());


    QString queryText= QString("DELETE FROM customers WHERE id=%1")
            .arg(customer.getId());

    Transaction::open();
    Transaction::run(queryText);
    Transaction::commit();
}

void Database::editCustomer(const Customer &customer)
{
    qDebug().noquote() <<  QString("Updating customer >%1< in database").arg(customer.concatedName());


    QSqlQuery query1;
    query1.prepare("UPDATE customersView "
                   "SET short=?, full=?, title=?, name=?, surname=? "
                   "WHERE customerID=?");
    query1.addBindValue(customer.getShortName());
    query1.addBindValue(customer.getFullName());
    query1.addBindValue(customer.getTitle());
    query1.addBindValue(customer.getName());
    query1.addBindValue(customer.getSurname());
    query1.addBindValue(customer.getId());

    QSqlQuery query2;
    query2.prepare("UPDATE customersView "
                   "SET address=? "
                   "WHERE customerID=?");
    query2.addBindValue(customer.getAddress());
    query2.addBindValue(customer.getId());

    Transaction::open();
    Transaction::run(query1);
    Transaction::run(query2);
    Transaction::commit();
}

void Database::saveCustomer(const Customer &customer)
{
    qDebug().noquote() <<  QString("Saving customer >%1< to database").arg(customer.concatedName());


    QSqlQuery query1;
    query1.prepare("INSERT INTO addresses (address) VALUES (?)");
    query1.addBindValue(customer.getAddress());
    QSqlQuery query2;
    query2.prepare("INSERT INTO customers (short, full, title, name, surname, addressID) "
                               "VALUES (?, ?, ?, ?, ?, LAST_INSERT_ID())");
    query2.addBindValue(customer.getShortName());
    query2.addBindValue(customer.getFullName());
    query2.addBindValue(customer.getTitle());
    query2.addBindValue(customer.getName());
    query2.addBindValue(customer.getSurname());

    Transaction::open();
    Transaction::run(query1);
    Transaction::run(query2);
    Transaction::commit();
}

TermModel *Database::getTermModel(TermType termType)
{
    QSqlTableModel model;
    model.setTable(termTable[termType]);
    model.select();
    // make sure the complete result set is fetched
    while(model.canFetchMore())
         model.fetchMore();

    //convert to term model
    TermModel* terms = new TermModel;
    for (int i = 0; i < model.rowCount(); ++i)
    {
        QSqlRecord rec = model.record(i);
        terms->insert(new TermItem(termType, rec.value("shortDesc").toString(), rec.value("longDesc").toString(), rec.value("id").toInt()));
    }

    return terms;
}

TermItem Database::getTerm(TermType termType, int id)
{
    QSqlTableModel model;
    model.setTable(termTable[termType]);
    model.setFilter(QString("id = %2").arg(id));
    model.select();

    //convert to term
    QSqlRecord rec = model.record(0);
    return TermItem(termType, rec.value("shortDesc").toString(), rec.value("longDesc").toString(), rec.value("id").toInt());
}

int Database::getNewOfferNumber(int uid)
{
    qDebug().noquote() << "Getting new offer number for user: " << uid;

    QString queryText = QString("select get_new_offer_number(%1) as 'newOfferNumber'").arg(uid);
    Transaction::open();
    QSqlQuery query = Transaction::run(queryText);
    if(query.isActive() == false)
    {
        throw DatabaseException("Something went wrong when retriving new offer number");
    }
    query.next();
    int newOfferNumber = query.value(0).toInt();
    Transaction::commit();
    qDebug().noquote().nospace() << "New offer numbwer = " << newOfferNumber;
    return newOfferNumber;
}

bool Database::isConnected()
{
    return QSqlDatabase::database().isOpen();
}

QHash<QString, int> Database::usersList()
{
    QHash<QString, int> userList;
    QSqlTableModel usersTable;

    usersTable.setTable("users");
    usersTable.select();

    // make sure the complete result set is fetched
    while (usersTable.canFetchMore())
         usersTable.fetchMore();

    for (int r = 0; r < usersTable.rowCount(); ++r)
        userList.insert(usersTable.data(usersTable.index(r,1)).toString(), usersTable.data(usersTable.index(r,0)).toInt());

    return userList;
}

void Database::createTerm(const TermItem& term)
{
    QString queryText;
    qDebug().noquote() << "Creating new term:" << term.shortDesc();

    queryText = QString("INSERT INTO %1 (shortDesc, longDesc) VALUES ('%3', '%4')")
            .arg(termTable[term.getType()])
            .arg(term.shortDesc())
            .arg(term.longDesc());

    Transaction::open();
    Transaction::run(queryText);
    Transaction::commit();
}

QString Database::mainAddress()
{
    QSqlTableModel model;
    model.setTable("addresses");
    model.setFilter(QString("id = 2"));
    model.select();

    if(model.rowCount() < 1)
    {
        qCritical() << "Company address not found!";
        return QString::null;
    }
    QSqlRecord rec = model.record(0);
    QString address = rec.value("address").toString();
    qDebug() << "Company address:" << address;
    return address;
}

QSqlRecord Database::getCustomerData(int id)
{
    QSqlTableModel model;
    model.setTable("customersView");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setFilter(QString("customerID = %1").arg(id));
    model.select();
    if(model.rowCount() < 1)
        throw DatabaseException(QString("Customer id = %1 not found").arg(id));
    return model.record(0);
}

QHash<TermType, QString> Database::getTermTable()
{
    return termTable;
}

void Database::dropConection()
{
    auto database = QSqlDatabase::addDatabase("QMYSQL");
    database.close();
}
