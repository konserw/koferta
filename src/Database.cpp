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

Database* Database::m_instance = nullptr;

QHash<TermType, QString> termTable
{
    {TermType::billing, "billingTerms"},
    {TermType::delivery, "deliveryTerms"},
    {TermType::deliveryDate, "deliveryDateTerms"},
    {TermType::offer, "offerTerms"}
};

Database::Database() :
    QObject(nullptr), m_database(nullptr)
{ }

Database::~Database()
{
    qDebug() << "Database class destruktor";
    dropConection();
}

Database *Database::instance()
{
    if(m_instance == nullptr)
        m_instance = new Database;

    return m_instance;
}

void Database::dropConection()
{
    qDebug() << "drop database connection";
    if(m_database != nullptr)
    {
        m_database->close();
        delete m_database;
        m_database = nullptr;
    }
}

void Database::setupDatabaseConnection(const QString &host, unsigned port, const QString& schema, const QString& user, const QString& password)
{
    qDebug() << "Setup database connection";
    dropConection();
    qDebug() << "Database object does not exist - creating it";
    if(QSqlDatabase::drivers().contains("QMYSQL"))
         m_database = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    else
    {
        qDebug() << "library paths: ";
        QStringList list = qApp->libraryPaths();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];

        qDebug() << "aviable drivers: ";
        list = QSqlDatabase::drivers();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];
        throw DatabaseException("Failed to create database object - QMYSQL driver not found.");
    }

    qDebug().noquote().nospace() << "Check if TCP socket " << host << ":" << port << " is reachable";
    QTcpSocket socket;
    socket.connectToHost(host, port);
    if(socket.waitForReadyRead() == false)
    {
        qCritical() << "Socket connection failed";
        emit changeStatus(tr("Wystąpił błąd - uruchom ponownie lub popraw dane połączenia"));
        return;
    }
    socket.disconnectFromHost();
    qDebug() << "Socket has been successfully opened";

    //fill in database connection details
    m_database->setHostName(host);
    m_database->setPort(port);
    m_database->setDatabaseName(schema);
    if(!user.isNull() && !password.isNull())
    {
        m_database->setUserName(user);
        m_database->setPassword(password);
    }

    qDebug().nospace().noquote() << "Set up connection details:\n"
             << "\t* Host:\t\t" << m_database->hostName() << "\n"
             << "\t* Port:\t\t" << m_database->port() << "\n"
             << "\t* UserName:\t" << m_database->userName() << "\n"
//             << "\t* Password:\t" << QCryptographicHash::hash(m_database->password().toUtf8(), QCryptographicHash::Sha1).toBase64() << "\n"
             << "\t* Schema:\t" << m_database->databaseName();

    //open database connection

    if(!m_database->open())
    {
        qCritical().nospace().noquote() << "Unable to connect to database! Details:\n"
        << "\t* connName:" << m_database->connectionName() << "\n"
        << "\t* driver:" << m_database->driverName() << "\n"
        << "\t* options:" << m_database->connectOptions() << "\n"
        << "\t* host:" << m_database->hostName() << "\n"
        << "\t* error number:" << m_database->lastError().number() << "\n"
        << "\t* database error:" << m_database->lastError().databaseText() << "\n"
        << "\t* driver error:" << m_database->lastError().driverText();

        emit changeStatus(tr("Połączenie z bazą danych nie powiodło się."));
        return;
    }

    emit changeStatus(tr("Połączono z bazą danych"));
    emit connectionSuccess();
}

void Database::setPassword(int uid, QString password)
{
    std::random_device rd;
    QString salt = QCryptographicHash::hash(QByteArray::number(uid + rd()), QCryptographicHash::Sha1).toBase64();
    qDebug() << "new salt for user" << uid << "is" << salt;

    QString queryText = QString("UPDATE users SET password='%1', salt='%2', resetPassword=0 WHERE id=%3")
            .arg(saltPassword(salt, password))
            .arg(salt)
            .arg(uid);

    Transaction::open();
    Transaction::run(queryText);
    Transaction::commit();
}

User Database::logIn(int uid, const QString &password)
{
    qDebug() << "Downloading user info for" << uid;

    QSqlTableModel usersTable;
    usersTable.setTable("users");
    usersTable.setFilter(QString("id = %1").arg(uid));
    usersTable.select();
    if(usersTable.rowCount() < 1)
    {
        qCritical().noquote() << "Invalid user selected";
        return User();
    }

    QSqlRecord r = usersTable.record(0);
    //qDebug() << "user record" << r;
    QString dbPassword = r.value("password").toString();
    QString salt = r.value("salt").toString();

    qDebug() << "User's salt:" << salt;
    if(saltPassword(salt, password) != dbPassword)
    {
        qCritical().noquote() << "Wrong password has been given";
        return User();
    }

    return User(
                r.value("id").toInt(),
                r.value("name").toString(),
                r.value("phone").toString(),
                r.value("mail").toString(),
                r.value("charForOfferSymbol").toString(),
                r.value("male").toBool(),
                r.value("resetPassword").toBool()
                );
}

void Database::saveOffer(const Offer &offer) const
{
    qDebug().noquote() << "Saving offer symbol:" << offer.symbol;
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
/* offerSymbol */   	.arg(offer.symbol)
/* userID */        	.arg(offer.getUser().getUid())
/* customerID */    	.arg(offer.customer.getIDorNull())
/* offerDate */     	.arg(offer.date.toString("yyyy-MM-dd"))
/* inquiryDate */   	.arg(offer.getInquiryDateSql())
/* InquiryNumber */ 	.arg(offer.getInquiryNumberSql())
/* deliveryTerms */ 	.arg(offer.getTermIDforDB(TermType::delivery))
/* deliveryDateTerms */	.arg(offer.getTermIDforDB(TermType::deliveryDate))
/* billingTerms */      .arg(offer.getTermIDforDB(TermType::billing))
/* offerTerms */        .arg(offer.getTermIDforDB(TermType::offer))
/* remarks */           .arg(escapedRemarks)
/* dExchangeRate */    	.arg(offer.getExchangeRateSql())
/* bPrintSpecs */       .arg(offer.printOptions.testFlag(Offer::printSpecs)? "1" : "0")
/* bPrintRawPrice */    .arg(offer.printOptions.testFlag(Offer::printRawPrice)? "1" : "0")
/* bPrintRawPricePLN */ .arg(offer.printOptions.testFlag(Offer::printRawPricePLN)? "1" : "0")
/* bPrintDiscount */  	.arg(offer.printOptions.testFlag(Offer::printDiscount)? "1" : "0")
/* bPrintPrice */       .arg(offer.printOptions.testFlag(Offer::printPrice)? "1" : "0")
/* bPrintNumber */		.arg(offer.printOptions.testFlag(Offer::printNumber)? "1" : "0");

    //save merchandise
    QSqlQuery query;
    query.prepare("INSERT INTO `savedOffersMerchandise` (`offerID`, `sequenceNumber`, `merchandiseID`, `quantity`, `discount`) "
                  "VALUES (LAST_INSERT_ID(), :seq, :mer, :ile, :rabat)");

    Merchandise* merchandise;
    const QList<Merchandise*>& merchandiseList = offer.merchandiseList->m_list;
    QVariantList seq, mer, ile, rabat;
    for(int i=0; i < merchandiseList.length(); ++i)
    {
        merchandise = merchandiseList[i];
        seq << i;
        mer << merchandise->id();
        ile << merchandise->ilosc();
        rabat << merchandise->rabat();
    }

    query.addBindValue(seq);
    query.addBindValue(mer);
    query.addBindValue(ile);
    query.addBindValue(rabat);

    Transaction::open();
    Transaction::run(queryText);
    Transaction::runBatch(query);
    Transaction::commit();
}

void Database::loadOffer(Offer* offer, int offerID)
{
    qDebug() << "Loading offer" << offerID;

    QSqlTableModel model;
    model.setTable("savedOffersFullView");
    model.setFilter(QString("offerID = '%1'").arg(offerID));
    if(model.select() == false)
        throw DatabaseException("Something went wrong while reading data from savedOffersFullView");
    QSqlRecord rec = model.record(0);

    offer->id = offerID; //need it?
    offer->setSymbol(rec.value("offerSymbol").toString());
    offer->date = rec.value("offerDate").toDate();
    if(!rec.value("customerID").isNull())
        offer->setCustomer(Customer(
                    rec.value("short").toString(),
                    rec.value("full").toString(),
                    rec.value("title").toString(),
                    rec.value("name").toString(),
                    rec.value("surname").toString(),
                    rec.value("address").toString(),
                    rec.value("customerID").toInt()
                    ));

    offer->setInquiryDate(rec.value("inquiryDate").toString());
    offer->setInquiryNumber(rec.value("inquiryNumber").toString());

    for(auto it = termTable.begin(); it != termTable.end(); ++it)
    {
        auto val = rec.value(it.value());
        if(!val.isNull())
            offer->setTerm(getTerm(it.key(), val.toInt()));
    }
    offer->setTerm(TermItem(TermType::remarks, QString::null, rec.value("remarks").toString()));

    QVariant exchange = rec.value("dExchangeRate");
    if(exchange.isNull())
        offer->setPln(false);
    else
    {
        offer->setPln(true);
        offer->setExchangeRate(exchange.toDouble());
    }

    Offer::PrintOptions options;
    options.setFlag(Offer::printSpecs, rec.value("bPrintSpecs").toBool());
    options.setFlag(Offer::printRawPrice, rec.value("bPrintRawPrice").toBool());
    options.setFlag(Offer::printRawPricePLN, rec.value("bPrintRawPricePLN").toBool());
    options.setFlag(Offer::printDiscount, rec.value("bPrintDiscount").toBool());
    options.setFlag(Offer::printPrice, rec.value("bPrintPrice").toBool());
    options.setFlag(Offer::printNumber, rec.value("bPrintNumber").toBool());
    offer->setPrintOptions(options);

    //merch list
    QString queryText = QString(
                        "SELECT * "
                        "FROM savedOffersMerchandiseView "
                        "WHERE offerID = %1 "
                        "ORDER BY sequenceNumber ASC"
                        ).arg(offerID);
    QSqlQuery query = Transaction::run(queryText);
    if(query.isActive() == false)
    {
        QString error = "Something went wrong when retriving merchandise list";
        throw DatabaseException(error);
    }

    int merchandiseCount = query.size();
    if(merchandiseCount > 0)
    {
        Merchandise* merchandise;
        offer->merchandiseList->beginInsertRows(QModelIndex(), 0, merchandiseCount - 1);
        while(query.next())
        {
            qDebug() << "Loading merchandise: " << query.value("merchandiseID").toString();
            merchandise = new Merchandise(
                        query.value("merchandiseID").toInt(),
                        query.value("code").toString(),
                        query.value("description").toString(),
                        query.value("price").toDouble(),
                        query.value("unit").toString() == "mb.");
            merchandise->setRabat(query.value("discount").toDouble());
            merchandise->setIlosc(query.value("quantity").toInt());
            offer->merchandiseList->m_list.append(merchandise);
        }
        offer->merchandiseList->endInsertRows();
    }

    qDebug() << "Done Loading!";
}

void Database::deleteCustomer(const Customer& customer) const
{
    qDebug().noquote() <<  QString("deleting customer >%1< from database").arg(customer.concatedName());


    QString queryText= QString("DELETE FROM customers WHERE id=%1")
            .arg(customer.id);

    Transaction::open();
    Transaction::run(queryText);
    Transaction::commit();
}

void Database::editCustomer(const Customer &customer) const
{
    qDebug().noquote() <<  QString("Updating customer >%1< in database").arg(customer.concatedName());


    QSqlQuery query1;
    query1.prepare("UPDATE customersView "
                   "SET short=?, full=?, title=?, name=?, surname=?"
                   "WHERE customerID=?");
    query1.addBindValue(customer.shortName);
    query1.addBindValue(customer.fullName);
    query1.addBindValue(customer.title);
    query1.addBindValue(customer.name);
    query1.addBindValue(customer.surname);
    query1.addBindValue(customer.id);

    QSqlQuery query2;
    query2.prepare("UPDATE customersView "
                   "SET address=?"
                   "WHERE customerID=?");
    query2.addBindValue(customer.address);
    query2.addBindValue(customer.id);

    Transaction::open();
    Transaction::run(query1);
    Transaction::run(query2);
    Transaction::commit();
}

void Database::saveCustomer(const Customer &customer) const
{
    qDebug().noquote() <<  QString("Saving customer >%1< to database").arg(customer.concatedName());


    QSqlQuery query1;
    query1.prepare("INSERT INTO addresses (address) VALUES (?)");
    query1.addBindValue(customer.address);
    QSqlQuery query2;
    query2.prepare("INSERT INTO customers (short, full, title, name, surname, addressID) "
                               "VALUES (?, ?, ?, ?, ?, LAST_INSERT_ID())");
    query2.addBindValue(customer.shortName);
    query2.addBindValue(customer.fullName);
    query2.addBindValue(customer.title);
    query2.addBindValue(customer.name);
    query2.addBindValue(customer.surname);

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

int Database::getNewOfferNumber(int uid) const
{
    qDebug().noquote() << "Getting new offer number for user: " << uid;

    QString queryText = QString("select get_new_offer_number(%1) as 'newOfferNumber'").arg(uid);
    Transaction::open();
    QSqlQuery query = Transaction::run(queryText);
    if(query.isActive() == false)
    {
        QString error = "Something went wrong when retriving new offer number";
        throw DatabaseException(error);
    }
    query.next();
    int newOfferNumber = query.value(0).toInt();
    Transaction::commit();
    qDebug().noquote().nospace() << "New offer numbwer = " << newOfferNumber;
    return newOfferNumber;
}

QString Database::getNewOfferSymbolForUser(const User& u) const
{
    int newOfferNumber = getNewOfferNumber(u.getUid());
/* Example: I1804P01
    1 znak - oznaczenie biznesu,
2 i 3 znak - rok,
4 i 5 znak – miesiąc,
    6 znak – osoba ofertująca (czyli A – Agata, M – Marek, P – Patryk itd.),
7 i 8 znak – numeracja ofert danej osoby w danym miesiącu,
*/
    return QString("I%1%2%3")
            .arg(QDate::currentDate().toString("yyMM"))
            .arg(u.getCharForOfferSymbol())
            .arg(QString::number(newOfferNumber).rightJustified(2, '0'));
}

bool Database::isConnected() const
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

LoadDialogMerchandiseListModel *Database::loadDialogMerchandiseListModel(QObject *parent)
{
    return new LoadDialogMerchandiseListModel(parent);
}

OfferSearchModel *Database::offerSearchModel(QObject *parent)
{
    auto model = new OfferSearchModel(parent);

    auto sourceModel = new QSqlTableModel(model);
    sourceModel->setTable("savedOffersView");
    sourceModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sourceModel->select();

    model->setSourceModel(sourceModel);
    return model;
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