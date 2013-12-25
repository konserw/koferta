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
        #include  <mysql.h>
    #else
       #include <mysql/mysql.h>
    #endif
#endif

void insert_towar(const QString& id, const QString& nazwa, double cena, const QString& jednostka)
{
    qDebug() <<  "Wstawianie towaru, id: " << id;

    QSqlQuery q;
    QString s;

    s = QString("SELECT nazwa FROM towar WHERE id='%1'").arg(id);
    EXEC_SILENT(s);

    if(q.next())
    {
        qDebug() <<  "Towar o id: " << id << " istnieje w bazie, nastapi jego nadpisanie!";
        s = QString("DELETE FROM towar WHERE id='%1'").arg(id);
        EXEC(s);
    }

    s = QString("INSERT INTO towar (id, nazwa, cena, jednostka) VALUES ('%1', '%2', '%3', '%4')").arg(id, nazwa, QString::number(cena), jednostka);

    EXEC_SILENT(s);
}

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

void insert_zapisane_towary(const QString& nr_oferty, const QString& kod, double ilosc, double rabat)
{
   // qqDebug()() << "\t\tZapisany towar kod: " << kod;

    QString s;
    QSqlQuery q;

    s = QString("INSERT INTO zapisane_towary (nr_oferty, kod, ilosc, rabat) VALUES ('%1', '%2', %3, %4)").arg(nr_oferty, kod, QString::number(ilosc), QString::number(rabat));
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
    QObject(parent), m_usersTable(nullptr)
{
    m_initialConnection = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL", "initialConnection"));
    m_initialConnection->setUserName("kOf_GetUsers");
    init(*m_initialConnection);

    m_usersTable = new QSqlTableModel(this, *m_initialConnection);
}

Database::~Database()
{
    delete m_initialConnection;
}

void Database::hostChanged(QString ip)
{
    qDebug() << "Host changed for:" << ip;
    emit changeStatus(tr("Łączenie z bazą danych na %1").arg(ip));

    m_initialConnection->setHostName(ip);

    login(*m_initialConnection);
    if(!m_initialConnection->isOpen())
    {
        emit changeStatus(tr("Połączenie z bazą danych na %1 nie powiodło się.").arg(ip));
        return;
    }
    emit changeStatus(tr("Połączono z bazą danych na %1").arg(ip));

    QStringList userList;

    m_usersTable->setTable("users");
    m_usersTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_usersTable->setFilter("");
    m_usersTable->select();

    // make sure the complete result set is fetched
    while (m_usersTable->canFetchMore())
         m_usersTable->fetchMore();

    for (int r = 0; r < m_usersTable->rowCount(); ++r)
        userList << m_usersTable->data(m_usersTable->index(r,1)).toString();

    emit newUsers(userList);
    //m_db->close();
}

void Database::connect(const QString &name, const QString &pass)
{
    qDebug() << "Downloading user info for:" << name;

    if(m_initialConnection->userName() != "kOf_GetUsers" || !m_initialConnection->isOpen())
    {
        qDebug() << "Reconnecting as kOf_GetUsers";

        m_initialConnection->close();
        m_initialConnection->setUserName("kOf_GetUsers");

        login(*m_initialConnection);
    }

    if(!m_initialConnection->isOpen())
    {
        emit changeStatus(tr("Połączenie z bazą danych na %1 jest zerwane!").arg(m_initialConnection->hostName()));
        return;
    }
    m_usersTable->setFilter(QString("name == '%1'").arg(name));
    m_usersTable->select();
    QSqlRecord r = m_usersTable->record(0);

    cUser currentUser(r.value("uid").toInt(), name, r.value("mail").toString(), r.value("adress").toString(), r.value("male").toBool(), r.value("nrOferty").toInt());

    QSqlDatabase finalConnection = QSqlDatabase::addDatabase("QMYSQL", "finalConnection");
    finalConnection.setHostName(m_initialConnection->hostName());
    finalConnection.setUserName(currentUser.dbName());
    finalConnection.setPassword(pass);

    qDebug() << "Logowanie jako " << name;
    login(finalConnection);

    if(!finalConnection.isOpen())
    {
        emit changeStatus(tr("%1: Błąd logowania jako %1").arg(finalConnection.hostName(), name));
        return;
    }

    emit connectionSuccess(currentUser);
}

void Database::init(QSqlDatabase &db)
{
    db.setDatabaseName("kOferta");
    db.setPort(3306);

#ifdef NOSSL
    qWarning() << "NOSSL defined, pomijam ustawianie bezpiecznego połączenia";
#else
    db.setConnectOptions("CLIENT_SSL=1;CLIENT_IGNORE_SPACE=1");

    QVariant v = db.driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "MYSQL*")==0)
    {
        MYSQL *handle = static_cast<MYSQL *>(v.data());
        if (handle != NULL)
        {
            mysql_ssl_set(handle, ":/client-key", ":/client-cert", ":/ca-cacert", NULL, NULL);
        }
    }
    else
    {
        QMessageBox::critical(nullptr, tr("Błąd"), tr("Bład sterownika bazy danych!\nNastąpi zamknięcie programu."));
        qWarning() << "invalid driver";

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

void Database::login(QSqlDatabase& db)
{
    if (db.open())
    {
        qWarning() << "Error! Unable to connect to database!";
        qDebug() << "connName:" << db.connectionName();
        qDebug() << "driver:" << db.driverName();
        qDebug() << "options:" << db.connectOptions();
        qDebug() << "host:" << db.hostName();
        qDebug() << "database error:" << db.lastError().databaseText();
        qDebug() << "driver error:" << db.lastError().driverText();
    }
}
