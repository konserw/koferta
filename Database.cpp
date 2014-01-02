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
#include <exception>
#include "User.h"
#include "Database.h"
#include "Macros.h"
#ifndef NOSSL
    #ifdef WIN32
        #include <my_global.h>
        #include <mysql.h>
    #else
       #include <mysql/mysql.h>
    #endif
#endif

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

    s = QString("DELETE FROM zapisane_towary WHERE nr_oferty = '%1'").arg(nr_oferty);
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

void insert_combo(const QString& typ, const QString& sh, const QString& lo)
{
    qDebug() << "Wstawianie opcji " << sh << " do kombosa " << typ;

    QString s;
    QSqlQuery q;

    s = QString("INSERT INTO %1 VALUES (DEFAULT, '%2', '%3')").arg(typ, sh, lo);
    EXEC_SILENT(s);
}

void Database::setupInitialConnection()
{
    m_initialConnection = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL", "initialConnection"));
    m_initialConnection->setDatabaseName("kOferta");
    m_initialConnection->setPort(3306);
    m_initialConnection->setUserName("kOf_GetUsers");

    setupSSL(*m_initialConnection);
}

Database::Database(QObject* parent) :
    QObject(parent), m_usersTable(nullptr)
{  
}

Database::~Database()
{
    m_initialConnection->close();
    delete m_initialConnection;
}

QStringList Database::getUsersList(const QSqlDatabase& db = QSqlDatabase())
{
    QStringList userList;

    if(m_usersTable)
        delete m_usersTable;
    m_usersTable = new QSqlTableModel(this, db);
    m_usersTable->setTable("users");
    m_usersTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_usersTable->select();

    // make sure the complete result set is fetched
    while (m_usersTable->canFetchMore())
         m_usersTable->fetchMore();

    int rows = m_usersTable->rowCount();
    qDebug() << "Users list count" << rows;

    for (int r = 0; r < rows; ++r)
        userList << m_usersTable->data(m_usersTable->index(r,1)).toString();

    return userList;
}

void Database::hostChanged(QString ip)
{
    qDebug() << "Host changed for:" << ip;
 //   emit changeStatus(tr("Łączenie z bazą danych na %1").arg(ip));

    m_initialConnection->setHostName(ip);

    if(!openDatabaseConnection(*m_initialConnection))
    {
        emit changeStatus(tr("Połączenie z bazą danych na %1 nie powiodło się.").arg(ip));
        return;
    }
    emit changeStatus(tr("Połączono z bazą danych na %1").arg(ip));

    QStringList userList = getUsersList(*m_initialConnection);
    emit newUsers(userList);
}

void Database::connect(const QString& name, const QString &pass)
{
    QString dbName = User::dbName(name);

    QSqlDatabase finalConnection = QSqlDatabase::addDatabase("QMYSQL"/*, "finalConnection"*/);
    finalConnection.setDatabaseName("kOferta");
    finalConnection.setPort(3306);
    finalConnection.setHostName(m_initialConnection->hostName());
    finalConnection.setUserName(dbName);
    finalConnection.setPassword(pass);
    setupSSL(finalConnection);

    qDebug() << "Loging in as" << dbName;
    if(!openDatabaseConnection(finalConnection))
    {
        emit changeStatus(tr("%1: Błąd logowania jako %2").arg(finalConnection.hostName(), name));
        return;
    }

    qDebug() << "Downloading user info for" << name;

    m_usersTable->setFilter(QString("name = '%1'").arg(name));
    m_usersTable->select();
    QSqlRecord r = m_usersTable->record(0);

    User currentUser(r.value("uid").toInt(), name, r.value("mail").toString(), r.value("adress").toString(), r.value("male").toBool(), r.value("nrOferty").toInt());

    emit connectionSuccess(currentUser);
}

void Database::setupSSL(QSqlDatabase &db)
{
#ifdef NOSSL
    Q_UNUSED(db);
    qWarning() << "NOSSL defined, pomijam ustawianie bezpiecznego połączenia";
#else
    db.setConnectOptions("CLIENT_SSL=1;CLIENT_IGNORE_SPACE=1");

    QVariant v = db.driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "MYSQL*")==0)
    {
        MYSQL *handle = *static_cast<MYSQL **>(v.data());
        if (handle != NULL)
        {
            try
            {
                qDebug() << "v.typename:" << v.typeName();
                qDebug() << "handle:" << handle;
                qDebug() << "mysql error:" << mysql_error(handle);

                mysql_ssl_set(handle, ":/client-key", ":/client-cert", ":/ca-cacert", NULL, NULL);
//                mysql_ssl_set(handle, "D:/git/build-kOferta/client-key.pem", "D:/git/build-kOferta/client-cert.pem", "D:/git/build-kOferta/ca-cacert.pem", "D:/git/build-kOferta/", "DHE-RSA-AES256-SHA");
            }
            catch (std::exception& e)
            {
                qCritical() << "[mysql_ssl_set] Standard exception: " << e.what();
            }
            catch(...)
            {
                qCritical() << "[mysql_ssl_set] Unknown exception";
            }
        }
        else
        {
            qCritical() << "invalid mysql handle - unable to setup ssl connection";
        }
    }
    else
    {
        QMessageBox::critical(nullptr, tr("Błąd"), tr("Bład sterownika bazy danych!\nNastąpi zamknięcie programu."));
        qCritical() << "invalid driver";

        qDebug() << "library paths: ";
        QStringList list = qApp->libraryPaths();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];

        qDebug() << "aviable drivers: ";
        list = QSqlDatabase::drivers();
        for(int i=0; i<list.size(); i++)
            qDebug() << "\t" << list[i];

        throw std::exception("invalid driver");
    }
#endif
}

bool Database::openDatabaseConnection(QSqlDatabase& db)
{
    if(!db.open())
    {
        qCritical() << "Error! Unable to connect to database!";
        qDebug() << "connName:" << db.connectionName();
        qDebug() << "driver:" << db.driverName();
        qDebug() << "options:" << db.connectOptions();
        qDebug() << "host:" << db.hostName();
        qDebug() << "error number:" << db.lastError().number();
        qDebug() << "database error:" << db.lastError().databaseText();
        qDebug() << "driver error:" << db.lastError().driverText();
        return false;
    }
    return true;
}
