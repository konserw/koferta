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
#include <QMessageBox>
#include <QInputDialog>
#include <QString>
#include <QtSql>
#include <QtDebug>
#include <QVariant>
#include <QMessageBox>
#include <QTcpSocket>
#include <QCryptographicHash>
#include <random>
#include "MainWindow.h"
#include "User.h"
#include "Customer.h"
#include "Merchandise.h"
#include "User.h"
#include "Database.h"
#include "LoadDialogMerchandiseListModelMySQL.h"
#include "Offer.h"
#include "MerchandiseListModel.h"

Database* Database::m_instance = nullptr;

// Returns HMAC-SHA1 encrypted signature composed of public key and secret base string
// function pasted from https://developersarea.wordpress.com/2014/11/27/cryptographic-hashes-in-qt-hmac-sha1-pbkdf2-md5/
// copyright Neha Gupta
// with modifications by Kamil Strzempowicz
QByteArray Database::hmacSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }
    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "\"
    // ascii characters 0x36 ("6") and 0x5c ("\") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    return QCryptographicHash::hash(total, QCryptographicHash::Sha1);
}

// wraper for hmacSha1 for salting passwords
QString Database::saltPassword(const QString& salt, const QString& password)
{
    return hmacSha1(password.toUtf8(), salt.toUtf8()).toBase64();
}

Database::Database() :
    QObject(nullptr)
{
    m_database = nullptr;
}

Database::~Database()
{
    qDebug() << "Database class destruktor";
    dropConection();
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

void Database::setupDatabaseConnection(const QString &host, unsigned port, const QString& schema)
{
    qDebug() << "Setup database connection";
    dropConection();
    qDebug() << "Database object does not exist - creating it";
    if(QSqlDatabase::drivers().contains("QMYSQL"))
         m_database = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Błąd"), QObject::tr("Bład sterownika bazy danych!"));

        qDebug() << "library paths: ";
        QStringList list = qApp->libraryPaths();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];

        qDebug() << "aviable drivers: ";
        list = QSqlDatabase::drivers();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];
        qFatal("Failed to create database object - QMYSQL driver not found.");
        //Application will close
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
    m_database->setUserName(SQL_USER);
    m_database->setPassword(SQL_PWD);

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

bool Database::setPassword(int uid, QString password)
{
    std::random_device rd;
    QString salt = QCryptographicHash::hash(QByteArray::number(uid + rd()), QCryptographicHash::Sha1).toBase64();
    qDebug() << "new salt for user" << uid << "is" << salt;

    QString queryText = QString("UPDATE user SET password='%1', salt='%2', resetPassword=0 WHERE uid=%3")
            .arg(saltPassword(salt, password))
            .arg(salt)
            .arg(uid);

    return transactionRun(queryText);
}

bool Database::logIn(int uid, const QString &password)
{
    qDebug() << "Downloading user info for" << uid;

    QSqlTableModel usersTable;
    usersTable.setTable("usersView");
    usersTable.setFilter(QString("uid = '%1'").arg(uid));
    usersTable.select();
    if(usersTable.rowCount() < 1)
    {
        qCritical().noquote() << "Invalid user selected";
        return false;
    }

    QSqlRecord r = usersTable.record(0);
    //qDebug() << "user record" << r;
    User::current().uid = r.value("uid").toInt();
    User::current().name = r.value("name").toString();
    User::current().phone = r.value("phone").toString();
    User::current().mail = r.value("mail").toString();
    User::current().address = r.value("address").toString();
    User::current().male = r.value("male").toBool();
    User::current().currentOfferNumber = r.value("currentOfferNumber").toInt();
    User::current().resetPassword = r.value("resetPassword").toBool();
    QString dbPassword = r.value("password").toString();
    QString salt = r.value("salt").toString();

    qDebug() << "User's salt:" << salt;
    return (saltPassword(salt, password) == dbPassword);
}

bool Database::transactionRun(const QString& queryText)
{
    QSqlQuery query;
    if(query.exec(queryText) == false)
    {
        qCritical().nospace().noquote()
                << "SQL query has failed!\n"
                << "\t* Query: " << queryText << "\n"
                << "\t* Error text: " <<  query.lastError().text();
        if(QSqlDatabase::database().rollback() == false)
        {
            qCritical().nospace().noquote()
                << "SQL transaction rollback has failed!\n"
                << "\t* Error text: " <<  QSqlDatabase::database().lastError().text();
        }
        return false;
    }
    return true;
}

bool Database::save(const Offer &offer) const
{
    qDebug().noquote() << "Saving offer number" << offer.numberWithYear;
    QString queryText;

    if(transactionOpen() == false)
        return false;

    if(offer.number == User::current().currentOfferNumber)
    {//fresh offer -> increment last offer number for user
        if(User::current().incrementOfferNumber() == false) //-> paste here?
            return false;
    }
    else
    {//old offer -> delete previous version from db
        queryText = QString("DELETE FROM zapisane WHERE nr_oferty = '%1'").arg(offer.numberWithYear);
        if(transactionRun(queryText) == false)
            return false;
        queryText = QString("DELETE FROM savedOffersMerchandise WHERE nr_oferty = '%1'").arg(offer.numberWithYear);
        if(transactionRun(queryText) == false)
            return false;
    }

    //save offer data itself
    QString escapedRemarks = offer.remarks;
    escapedRemarks.replace("\'", "\\\'");
    queryText = QString("INSERT INTO zapisane "
                        "(nr_oferty, id_klienta, data, uid, dostawa, termin, platnosc, oferta, uwagi, zapytanie_data, zapytanie_nr, dExchangeRate) "
                        "VALUES ('%1', %2, '%3', %4, %5, %6, %7, %8, '%9', %10, %11, %12)")
            .arg(offer.numberWithYear)
            .arg(offer.customer.id)
            .arg(offer.date.toString("dd.MM.yyyy"))
            .arg(User::current().getUid())
            .arg(offer.shippingTerm.id())
            .arg(offer.shipmentTime.id())
            .arg(offer.paymentTerm.id())
            .arg(offer.offerTerm.id())
            .arg(escapedRemarks)
            .arg(offer.getInquiryDateSql())
            .arg(offer.getInquiryNumberSql())
            .arg(offer.getExchangeRateSql());
    if(transactionRun(queryText) == false)
        return false;

    //save merchandise
    QSqlQuery query;
    query.prepare("INSERT INTO `savedOffersMerchandise` (`nr_oferty`, `sequenceNumber`, `merchandise_id`, `ilosc`, `rabat`) VALUES (:nr, :seq, :mer, :ile, :rabat)");

    Merchandise* merchandise;
    const QList<Merchandise*>& merchandiseList = offer.merchandiseList->m_list;
    QVariantList nr, seq, mer, ile, rabat;
    for(int i=0; i < merchandiseList.length(); ++i)
    {
        merchandise = merchandiseList[i];
        nr << offer.numberWithYear;
        seq << i;
        mer << merchandise->id();
        ile << merchandise->ilosc();
        rabat << merchandise->rabat();
    }

    query.addBindValue(nr);
    query.addBindValue(seq);
    query.addBindValue(mer);
    query.addBindValue(ile);
    query.addBindValue(rabat);

    if(query.execBatch() == false)
    {
        qCritical().nospace().noquote()
                << "SQL query has failed!\n"
                << "\t* Last query: " << query.lastQuery() << "\n"
                << "\t* Error text: " <<  query.lastError().text();
        if(QSqlDatabase::database().rollback() == false)
        {
            qCritical().nospace().noquote()
                << "SQL transaction rollback has failed!\n"
                << "\t* Error text: " <<  QSqlDatabase::database().lastError().text();
        }
        return false;
    }

    return transactionClose();
}

void Database::loadOffer(Offer* offer, const QString& offerId)
{
    QSqlTableModel model;
    model.setTable("savedOffersFullView");
    model.setFilter(QString("number = '%1'").arg(offerId));
    model.select();
    QSqlRecord rec = model.record(0);

    offer->numberWithYear = offerId;
    offer->date = QDate::fromString(rec.value("date").toString(), "dd.MM.yyyy");
    offer->setCustomer(Customer(
                rec.value("short").toString(),
                rec.value("full").toString(),
                rec.value("tytul").toString(),
                rec.value("imie").toString(),
                rec.value("nazwisko").toString(),
                rec.value("adres").toString(),
                rec.value("id").toInt()
                ));

    offer->setInquiryDate(rec.value("zapytanie_data").toString());
    offer->setInquiryNumber(rec.value("zapytanie_nr").toString());

    QVariant exchange = rec.value("dExchangeRate");
    if(exchange.isNull())
        offer->setPln(false);
    else
    {
        offer->setPln(true);
        offer->setExchangeRate(exchange.toDouble());
    }

    offer->setTerm(getTerm(TermItem::termShipping, rec.value("dostawa").toInt()));
    offer->setTerm(getTerm(TermItem::termShipmentTime, rec.value("termin").toInt()));
    offer->setTerm(getTerm(TermItem::termPayment, rec.value("platnosc").toInt()));
    offer->setTerm(getTerm(TermItem::termOffer, rec.value("oferta").toInt()));
    offer->setTerm(TermItem(TermItem::termRemarks, QString::null, rec.value("uwagi").toString()));

    offer->merchandiseList->loadOffer(offerId); // do przerobienia
}

void Database::changePasswordDialog()
{
    int uid = User::current().getUid();
    QString password = QInputDialog::getText(nullptr, tr("Zmiana hasła"), tr("Proszę wprowadzić nowe hasło"), QLineEdit::Password);
    if(password.isEmpty() || password.isNull())
    {
        qDebug() << "Skipped password change";
    }
    else
    {
        if(Database::instance()->setPassword(uid, password))
        {
            qDebug() << "Password updated";
            QMessageBox::information(nullptr, tr("Hasło zmienione"), tr("Nowe hasło zostało poprawnie zapisane w baziedanych."));
        }
        else
        {
            qDebug() << "Some error during password change occured";
            QMessageBox::warning(nullptr, tr("Hasło nie zmienione"), tr("Wystąpił błąd podczas zmiany hasła.\nHasło nie zostało zmienione"));
        }
    }
}

bool Database::transactionOpen()
{
    if(QSqlDatabase::database().transaction() == false)
    {
        qCritical().nospace().noquote()
                << "SQL transaction has failed!\n"
                << "\t* Error text: " <<  QSqlDatabase::database().lastError().text();
        return false;
    }
    return true;
}

bool Database::transactionClose()
{
    if(QSqlDatabase::database().commit() == false)
    {
        qCritical().nospace().noquote()
                << "SQL transaction commit has failed!\n"
                << "\t* Error text: " <<  QSqlDatabase::database().lastError().text();
        if(QSqlDatabase::database().rollback() == false)
        {
            qCritical().nospace().noquote()
                << "SQL transaction rollback has failed!\n"
                << "\t* Error text: " <<  QSqlDatabase::database().lastError().text();
        }
        return false;
    }
    return true;
}

bool Database::save(const Customer &client) const
{
    qDebug().noquote() <<  QString("Saving client >%1< to database").arg(client.concatedName());

    if(transactionOpen() == false)
        return false;

    QString queryText= QString("INSERT INTO klient (short, full, tytul, imie, nazwisko, adres) VALUES ('%1', '%2', '%3', '%4', '%5', '%6')").arg(client.shortName, client.fullName, client.title, client.name, client.surname, client.address);
    if(transactionRun(queryText) == false)
        return false;

    return transactionClose();
}

TermModel *Database::getTermModel(TermItem::TermType termType)
{
    QSqlTableModel model;
    model.setTable("terms");
    model.setFilter(QString("termType = %1").arg(termType));
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

TermItem Database::getTerm(TermItem::TermType termType, int id)
{
    QSqlTableModel model;
    model.setTable("terms");
    model.setFilter(QString("termType = %1 AND id = %2").arg(termType).arg(id));
    model.select();

    //convert to term
    QSqlRecord rec = model.record(0);
    return TermItem(termType, rec.value("shortDesc").toString(), rec.value("longDesc").toString(), rec.value("id").toInt());
}

QSqlQuery Database::transactionQuery(const QString &queryText)
{
    QSqlQuery query;
    if(query.exec(queryText) == false)
    {
        qCritical().nospace().noquote()
                << "SQL query has failed!\n"
                << "\t* Query: " << queryText << "\n"
                << "\t* Error text: " <<  query.lastError().text();
        if(QSqlDatabase::database().rollback() == false)
        {
            qCritical().nospace().noquote()
                << "SQL transaction rollback has failed!\n"
                << "\t* Error text: " <<  QSqlDatabase::database().lastError().text();
        }
    }
    return query;
}

Database *Database::instance()
{
    if(m_instance == nullptr)
        m_instance = new Database;

    return m_instance;
}

bool Database::setCurrentOfferNumber(int offerNumber)
{
    qDebug().noquote() << "Updating last offer number to:" << offerNumber;

    if(transactionOpen() == false)
        return false;

    QString queryText = QString("UPDATE user SET currentOfferNumber=%1 WHERE uid=%2").arg(offerNumber).arg(User::current().getUid());
    if(transactionRun(queryText) == false)
        return false;

    return transactionClose();
}

QHash<QString, int> Database::usersList()
{
    QHash<QString, int> userList;
    QSqlTableModel usersTable;

    usersTable.setTable("user");
    usersTable.select();

    // make sure the complete result set is fetched
    while (usersTable.canFetchMore())
         usersTable.fetchMore();

    for (int r = 0; r < usersTable.rowCount(); ++r)
        userList.insert(usersTable.data(usersTable.index(r,1)).toString(), usersTable.data(usersTable.index(r,0)).toInt());

    return userList;
}

bool Database::createTerm(const TermItem& term)
{
    QString queryText;
    qDebug().noquote() << "Creating new term:" << term.shortDesc();

    if(transactionOpen() == false)
        return false;

    queryText = QString("SELECT MAX(id) FROM terms WHERE termType = %1").arg(term.getType());
    QSqlQuery query = transactionQuery(queryText);
    if(query.isActive() == false)
        return false;

    query.next();
    int id = query.value(0).toInt() + 1;

    queryText = QString("INSERT INTO terms(termType, id, shortDesc, longDesc) VALUES (%1, %2, '%3', '%4')")
            .arg(term.getType())
            .arg(id)
            .arg(term.shortDesc())
            .arg(term.longDesc());
    if(transactionRun(queryText) ==false)
        return false;

    return transactionClose();
}

TermItem Database::paymentTerm(int id)
{
    return getTerm(TermItem::termPayment, id);
}

TermItem Database::shippingTerm(int id)
{
    return getTerm(TermItem::termShipping, id);
}

TermItem Database::shipmentTime(int id)
{
    return getTerm(TermItem::termShipmentTime, id);
}

TermItem Database::offerTerm(int id)
{
    return getTerm(TermItem::termOffer, id);
}

QList<Merchandise *> Database::loadOfferMerchandise(const QString &number)
{
    QList<Merchandise *> list;
    Merchandise* merchandise;

    if(transactionOpen() == false)
        return list;

    QString queryText = QString(
                        "SELECT * "
                        "FROM savedOffersMerchandiseView "
                        "WHERE nr_oferty = '%1' "
                        "ORDER BY sequenceNumber ASC"
                        ).arg(number);
    QSqlQuery query = transactionQuery(queryText);
    if(query.isActive() == false)
        return list;
    while(query.next())
    {
        merchandise = new Merchandise(query.value("merchandise_id").toInt(), query.value("code").toString(), query.value("description").toString(), query.value("price").toDouble(), query.value("unit").toString() == "mb.");
        merchandise->setRabat(query.value("rabat").toDouble());
        merchandise->setIlosc(query.value("ilosc").toInt());
        list.append(merchandise);
    }

    transactionClose();

    return list;
}

LoadDialogMerchandiseListModel *Database::loadDialogMerchandiseListModel(QObject *parent)
{
    return new LoadDialogMerchandiseListModelMySQL(parent);
}

QString Database::mainAddress()
{
    QSqlTableModel model;
    model.setTable("address");
    model.setFilter(QString("id = 2"));
    model.select();

    QSqlRecord rec = model.record(0);
    return rec.value("address").toString();
}
