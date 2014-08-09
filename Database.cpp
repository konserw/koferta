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
#include "User.h"
#include "Database.h"
#include "Macros.h"

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
