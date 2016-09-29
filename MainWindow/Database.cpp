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

#include <QString>
#include <QtSql>
#include <QtDebug>
#include <QVariant>
#include <QMessageBox>
#include <QSettings>
#include <QTcpSocket>
#include <QCryptographicHash>
#include <QProgressDialog>

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

Database::Database() :
    QObject(nullptr)
{
    m_progressDialog = nullptr;
    m_database = nullptr;
    m_socket = nullptr;
}

Database::~Database()
{
    qDebug() << "Database class destruktor";
    dropConection();
    delete m_progressDialog;
    delete m_socket;
}

void Database::dropConection()
{
    if(m_database != nullptr)
    {
        m_database->close();
        delete m_database;
        m_database = nullptr;
    }
}

void Database::setupDatabaseConnection(const QString &host, unsigned port, const QString& schema)
{
    createMysqlDatabase();

    m_database->setHostName(host);
    m_database->setPort(port);
    m_database->setDatabaseName(schema);
    m_database->setUserName(SQL_USER);
    m_database->setPassword(SQL_PWD);
    qWarning() << "user: " << SQL_USER << " pwd: " << SQL_PWD;

    qDebug().nospace().noquote() << "Set up connection details:\n"
             << "\t* Host:\t\t" << m_database->hostName() << "\n"
             << "\t* Port:\t\t" << m_database->port() << "\n"
             << "\t* UserName:\t" << m_database->userName() << "\n"
             << "\t* Password:\t" << QCryptographicHash::hash(m_database->password().toUtf8(), QCryptographicHash::Sha1).toBase64() << "\n"
             << "\t* Schema:\t" << m_database->databaseName();

    waitForTunnel(host, port);
}

void Database::logIn(const QString &user, const QString &password)
{
    //TODO:
    //implement password checking for koferta users
    //and generate User object
}


void Database::tunnelCancel()
{
    delete m_socket;
    m_socket = nullptr;
    qDebug() << "ssh: canceled by user";
    failedTunnel(QProcess::UnknownError);
}

void Database::openDatabaseConnection()
{
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

        emit changeStatus(tr("Połączenie z bazą danych powiodło się."));
        emit connectionFail();
        return;
    }

    emit changeStatus(tr("Połączono z bazą danych"));
    emit connectionSuccess();
}

void Database::failedTunnel(QProcess::ProcessError error) //nie dziala ?
{
    if(m_progressDialog != nullptr)
    {
        m_progressDialog->cancel();
        delete m_progressDialog;
        m_progressDialog = nullptr;
    }
    qDebug() << "ssh tunnel error:" << error;
    emit changeStatus(tr("Utworzenie tunelu do hosta %1 nie powiodło się."));
    emit connectionFail();
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

    if(offer.number == User::current()->currentOfferNumber)
    {//fresh offer -> increment last offer number for user
        if(User::current()->incrementOfferNumber() == false) //-> paste here?
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
                        "(nr_oferty, id_klienta, data, uid, dostawa, termin, platnosc, oferta, uwagi, zapytanie_data, zapytanie_nr) "
                        "VALUES ('%1', %2, '%3', %4, %5, %6, %7, %8, '%9', %10, %11)")
            .arg(offer.numberWithYear)
            .arg(offer.customer.id)
            .arg(offer.date.toString("dd.MM.yyyy"))
            .arg(User::current()->getUid())
            .arg(offer.shippingTerm.id())
            .arg(offer.shipmentTime.id())
            .arg(offer.paymentTerm.id())
            .arg(offer.offerTerm.id())
            .arg(escapedRemarks)
            .arg(offer.getInquiryDateSql())
            .arg(offer.getInquiryNumberSql());
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

    offer->inquiryDate = rec.value("zapytanie_data").toString();
    offer->inquiryNumber = rec.value("zapytanie_nr").toString();

    offer->setTerm(getTerm(TermItem::termShipping, rec.value("dostawa").toInt()));
    offer->setTerm(getTerm(TermItem::termShipmentTime, rec.value("termin").toInt()));
    offer->setTerm(getTerm(TermItem::termPayment, rec.value("platnosc").toInt()));
    offer->setTerm(getTerm(TermItem::termOffer, rec.value("oferta").toInt()));
    offer->setTerm(TermItem(TermItem::termRemarks, QString::null, rec.value("uwagi").toString()));

    offer->merchandiseList->loadOffer(offerId); // do przerobienia
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

void Database::createMysqlDatabase()
{
    if(m_database != nullptr)
        return;

    if(QSqlDatabase::drivers().contains("QMYSQL"))
         m_database = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Błąd"), QObject::tr("Bład sterownika bazy danych!"));
        qCritical() << "invalid driver";

        qDebug() << "library paths: ";
        QStringList list = qApp->libraryPaths();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];

        qDebug() << "aviable drivers: ";
        list = QSqlDatabase::drivers();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];
        emit driverFail();
    }
}

void Database::waitForTunnel(const QString& host, unsigned port)
{
    qDebug() << "Waiting for tunnel";
    m_socket = new QTcpSocket;

    m_progressDialog = new QProgressDialog(tr("Trwa łączenie z bazą danych, prosze czekać."), tr("Anuluj"), 0, 0);
    m_progressDialog->setWindowModality(Qt::ApplicationModal);

    connect(m_socket, &QTcpSocket::readyRead, this, &Database::tunelOpened);
    connect(m_socket, &QTcpSocket::connected, this, &Database::socketConnected);
    connect(m_progressDialog, &QProgressDialog::canceled, this, &Database::tunnelCancel);

    m_progressDialog->open();

    m_socket->connectToHost(host, port);
    qDebug() << "Connecting socket";
}

void Database::socketConnected()
{
    qDebug() << "socket connected";
}

void Database::tunelOpened()
{
    emit changeStatus(tr("Tworzenie tunelu zakończone powodzeniem"));
    qDebug() << "ssh tunnel opened ";
    m_socket->disconnectFromHost();
    delete m_socket;
    m_socket = nullptr;
    m_progressDialog->accept();
    delete m_progressDialog;
    m_progressDialog = nullptr;
    openDatabaseConnection();
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

User* Database::userInfo(const QString& userName)
{
    qDebug() << "Downloading user info for" << userName;

    QSqlTableModel usersTable;
    usersTable.setTable("usersView");
    usersTable.setFilter(QString("dbName = '%1'").arg(userName));
    usersTable.select();
 //   qDebug() << "users table row count after filter:" << usersTable.rowCount();
    if(usersTable.rowCount() < 1)
    {
        qCritical().noquote() << "Invalid user selected:" << userName;
        return nullptr;
    }

    QSqlRecord r = usersTable.record(0);
    //qDebug() << "user record" << r;
    return new User(r.value("uid").toInt(), r.value("name").toString(), r.value("phone").toString(), r.value("mail").toString(), r.value("address").toString(), r.value("male").toBool(), r.value("currentOfferNumber").toInt());
}

bool Database::setCurrentOfferNumber(int offerNumber)
{
    qDebug().noquote() << "Updating last offer number to:" << offerNumber;

    if(transactionOpen() == false)
        return false;

    QString queryText = QString("UPDATE user SET currentOfferNumber=%1 WHERE uid=%2").arg(offerNumber).arg(User::current()->getUid());
    if(transactionRun(queryText) == false)
        return false;

    return transactionClose();
}

//used to fill combobox in offersearch
QStringList Database::usersList()
{
    QStringList userList;
    QSqlTableModel usersTable;

    usersTable.setTable("user");
    usersTable.select();

    // make sure the complete result set is fetched
    while (usersTable.canFetchMore())
         usersTable.fetchMore();

    for (int r = 0; r < usersTable.rowCount(); ++r)
        userList << usersTable.data(usersTable.index(r,1)).toString();

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
