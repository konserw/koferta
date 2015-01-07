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
#include "Merchandise.h"
#include "User.h"
#include "Database.h"
#include "functions.h"
#include "LoadDialogMerchandiseListModelMySQL.h"

void insert_klient(const QString& skrot, const QString& full, const QString& tytul, const QString& imie, const QString& nazwisko, const QString& adres)
{
    qDebug() <<  "Wstawianie klienta, nazwisko: " << nazwisko;

    QString s;
    QSqlQuery q;

    s = QString("INSERT INTO klient (short, full, tytul, imie, nazwisko, adres) VALUES ('%1', '%2', '%3', '%4', '%5', '%6')").arg(skrot, full, tytul, imie, nazwisko, adres);
    EXEC_SILENT(s);
}

void insert_zapisane(const QString& nr_oferty, int id_klienta, const QString& data, int uid, const QString& zapytanie_data, const QString& zapytanie_nr, int dostawa, int termin, int platnosc, int oferta, const QString& uwagi)
{
    qDebug() <<  "Zapisywanie ofery nr: " << nr_oferty;

    QString s;
    QSqlQuery q;
    QString rep = uwagi;
    rep.replace("\'", "\\\'");

    s = QString("DELETE FROM zapisane WHERE nr_oferty = '%1'").arg(nr_oferty);
    EXEC_SILENT(s);

    s = "INSERT INTO zapisane "
        "(nr_oferty, id_klienta, data, uid, dostawa, termin, platnosc, oferta, uwagi, zapytanie_data, zapytanie_nr)";
    s += QString(" VALUES ('%1', %2, '%3', %4, %5, %6, %7, %8, '%9'").arg(
                nr_oferty, QString::number(id_klienta), data, QString::number(uid), QString::number(dostawa), QString::number(termin), QString::number(platnosc), QString::number(oferta), rep);
    if(zapytanie_data.isNull())
        s += ", NULL";
    else
        s += QString(", '%1'").arg(zapytanie_data);
    if(zapytanie_nr.isNull())
        s += ", NULL";
    else
        s += QString(", '%1'").arg(zapytanie_nr);
    s += ")";

    EXEC_SILENT(s);
}

Database::Database(QObject* parent) :
    QObject(parent)
{  
    QSettings settings;
    settings.beginGroup("connection");

    m_sslEnabled = settings.value("SSL enabled").toBool();
    m_host = settings.value("selected host", "localhost").toString();
    if(settings.value("testDB").toBool())
        m_schema = "kOferta_devel";
    else
        m_schema = "kOferta";

    settings.endGroup();

    m_database = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
}

Database::~Database()
{
    delete m_database;
}



void Database::connect(const QString& name, const QString &pass)
{
    QString databaseUserName = User::dbName(name);

    setupConnection(databaseUserName, pass);

    qDebug() << "Loging in as" << databaseUserName;
    if(!openDatabaseConnection())
    {
        emit changeStatus(tr("%1: Błąd logowania jako %2").arg(m_host, name));
        return;
    }

    emit connectionSuccess(userInfo(name));
}

void Database::getUsersList()
{
    setupConnection("kOf_GetUsers");

    if(!openDatabaseConnection())
    {
        emit changeStatus(tr("Połączenie z bazą danych %1 na %2 nie powiodło się.").arg(m_schema, m_host));
        return;
    }
    emit changeStatus(tr("Połączono z bazą danych %1 na %2").arg(m_schema, m_host));

    QStringList userList = usersList();
    emit newUsers(userList);
}

void Database::setupConnection(const QString& user, const QString pass)
{
    emit changeStatus(tr("Łączenie z bazą danych %1 na %2").arg(m_schema, m_host)); //??

    m_database->setHostName(m_host);
    m_database->setPort(3306);
    m_database->setDatabaseName(m_schema);
    m_database->setUserName(user);
    m_database->setPassword(pass);

    if(m_sslEnabled)
    {
        setupSSL();
    }
    else
    {
        qWarning() << "SSL has been disabled";
    }
}

TermModel *Database::getTermModel(Database::TermType termType)
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
        terms->insert(new TermItem(rec.value("id").toInt(), rec.value("shortDesc").toString(), rec.value("longDesc").toString()));
    }

    return terms;
}

TermItem Database::getTerm(Database::TermType termType, int id)
{
    QSqlTableModel model;
    model.setTable("terms");
    model.setFilter(QString("termType = %1 AND id = %2").arg(termType).arg(id));
    model.select();

    //convert to term
    QSqlRecord rec = model.record(0);
    return TermItem(rec.value("id").toInt(), rec.value("shortDesc").toString(), rec.value("longDesc").toString());
}

QStringList Database::usersList()
{
    QStringList userList;
    QSqlTableModel usersTable;

    usersTable.setTable("users");
    usersTable.select();

    // make sure the complete result set is fetched
    while (usersTable.canFetchMore())
         usersTable.fetchMore();

    int rows = usersTable.rowCount();
    qDebug() << "Users list count" << rows;

    for (int r = 0; r < rows; ++r)
        userList << usersTable.data(usersTable.index(r,1)).toString();

    return userList;
}

User Database::userInfo(const QString &name)
{
    qDebug() << "Downloading user info for" << name;

    QSqlTableModel usersTable;
    usersTable.setTable("users");
    usersTable.setFilter(QString("name = '%1'").arg(name));
    usersTable.select();
    QSqlRecord r = usersTable.record(0);

    return User(r.value("uid").toInt(), name, r.value("phone").toString(), r.value("mail").toString(), r.value("adress").toString(), r.value("male").toBool(), r.value("nrOferty").toInt());
}

void Database::createTerms(Database::TermType type, const QString &shortDesc, const QString &longDesc)
{
    QSqlQuery q;
    QString queryText;
    QSqlDatabase db = QSqlDatabase::database();

    db.transaction();

    queryText = QString("SELECT MAX(id) FROM terms WHERE termType = %1").arg(type);
    if(!q.exec(queryText))
    {
        db.rollback();
        QString error = q.lastError().text();
        QMessageBox::warning(nullptr,
                             tr("Database Write Error"),
                             tr("The database reported an error: %1")
                             .arg(error)
                             );
        qDebug() << "Following query failed with reason: " << error;
        qDebug() << queryText;
        return;
    }
    q.next();
    int id = q.value(0).toInt() + 1;

    queryText = QString("INSERT INTO terms(termType, id, shortDesc, longDesc) VALUES (%1, %2, '%3', '%4')")
            .arg(type)
            .arg(id)
            .arg(shortDesc)
            .arg(longDesc);
    if(!q.exec(queryText))
    {
        db.rollback();
        QString error = q.lastError().text();
        QMessageBox::warning(nullptr,
                             tr("Database Write Error"),
                             tr("The database reported an error: %1")
                             .arg(error)
                             );
        qDebug() << "Following query failed with reason: " << error;
        qDebug() << queryText;
        return;
    }

    db.commit();
}

TermModel* Database::paymentTermsModel()
{
    return getTermModel(termPayment);
}

TermModel* Database::shippingTermsModel()
{
    return getTermModel(termShipping);
}

TermModel* Database::shipmentTimeModel()
{
    return getTermModel(termShipmentTime);
}

TermModel* Database::offerTermsModel()
{
    return getTermModel(termOffer);
}

TermItem Database::paymentTerm(int id)
{
    return getTerm(termPayment, id);
}

TermItem Database::shippingTerm(int id)
{
    return getTerm(termShipping, id);
}

TermItem Database::shipmentTime(int id)
{
    return getTerm(termShipmentTime, id);
}

TermItem Database::offerTerm(int id)
{
    return getTerm(termOffer, id);
}

void Database::saveOfferMerchandise(const QString &offerId, const QList<Merchandise *> &merchandise)
{
    QString error;
    QSqlDatabase db;
    db.transaction();

    QSqlQuery query;
    if(!query.exec(QString("DELETE FROM savedOffersMerchandise WHERE nr_oferty = '%1'").arg(offerId)))
    {
        error = query.lastError().text();
        db.rollback();
        qWarning() << "Delete query execution failed!";
        qDebug() << "Query string:" << query.lastQuery();
        qDebug() << "Error string:" << error;
        QMessageBox::critical(nullptr, tr("Błąd"), tr("Wystąpił nastepujący bład podczas zapisu oferty do bazy danych:\n%1").arg(error));
        return;
    }

    query.clear();
    query.prepare("INSERT INTO `savedOffersMerchandise` (`nr_oferty`, `sequenceNumber`, `merchandise_id`, `ilosc`, `rabat`) VALUES (:nr, :seq, :mer, :ile, :rabat)");

    Merchandise* merch;
    QVariantList nr, seq, mer, ile, rabat;
    for(int i=0; i<merchandise.length(); ++i)
    {
        merch = merchandise[i];
        nr << offerId;
        seq << i;
        mer << merch->id();
        ile << merch->ilosc();
        rabat << merch->rabat();
    }

    query.addBindValue(nr);
    query.addBindValue(seq);
    query.addBindValue(mer);
    query.addBindValue(ile);
    query.addBindValue(rabat);

    if(query.execBatch())
    {
        db.commit();
    }
    else
    {
        error = query.lastError().text();
        db.rollback();
        qWarning() << "Database delete error:" << error;
        QMessageBox::critical(nullptr, tr("Błąd"), tr("Wystąpił nastepujący bład podczas zapisu oferty do bazy danych:\n%1").arg(error));
    }
}

QList<Merchandise *> Database::loadOfferMerchandise(const QString &number)
{
    QList<Merchandise *> list;
    Merchandise* merchandise;

    QSqlQuery query;
    query.exec(QString(
                        "SELECT * "
                        "FROM savedOffersMerchandiseView "
                        "WHERE nr_oferty = '%1' "
                        "ORDER BY sequenceNumber ASC"
                        ).arg(number));
//    qDebug() << query.lastQuery();
//    qDebug() << query.lastError();
    while(query.next())
    {
        merchandise = new Merchandise(query.value("merchandise_id").toInt(), query.value("code").toString(), query.value("description").toString(), query.value("price").toDouble(), query.value("unit").toString() == "mb.");
        merchandise->setRabat(query.value("rabat").toDouble());
        merchandise->setIlosc(query.value("ilosc").toInt());
        list.append(merchandise);
    }

    return list;
}

LoadDialogMerchandiseListModel *Database::loadDialogMerchandiseListModel(QObject *parent)
{
    return new LoadDialogMerchandiseListModelMySQL(parent);
}

void Database::setupSSL()
{
#ifdef WIN32
    m_database->setConnectOptions("CLIENT_SSL=1");//;CLIENT_IGNORE_SPACE=1");
    QString filePath = QString("%1/certs/%2").arg(qApp->applicationDirPath());
    m_database->setSslCertificateCaFilename(filePath.arg("ca-cert.pem"));
    m_database->setSslCertificateFilename(filePath.arg("client-cert.pem"));
    m_database->setSslKeyFilename(filePath.arg("client-key.pem"));
#endif
}

bool Database::openDatabaseConnection()
{
    if(!m_database->open())
    {
        qCritical() << "Error! Unable to connect to database!";
        qDebug() << "connName:" << m_database->connectionName();
        qDebug() << "driver:" << m_database->driverName();
        qDebug() << "options:" << m_database->connectOptions();
        qDebug() << "host:" << m_database->hostName();
        qDebug() << "error number:" << m_database->lastError().number();
        qDebug() << "database error:" << m_database->lastError().databaseText();
        qDebug() << "driver error:" << m_database->lastError().driverText();
        return false;
    }

    return true;
}
