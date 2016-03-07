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

#include "Merchandise.h"
#include "User.h"
#include "Database.h"
#include "LoadDialogMerchandiseListModelMySQL.h"

Database* Database::m_instance = nullptr;

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
    m_database = nullptr;
    tunnelProcess = nullptr;
}

Database::~Database()
{
    dropConection();
    if(tunnelProcess != nullptr)
    {
        tunnelProcess->kill();
        tunnelProcess->waitForFinished();
        delete tunnelProcess;
    }
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

void Database::setupDatabaseConnection(const QString& keyFile, const QString &pass)
{
    readSettings();
    createMysqlDatabase();

    m_keyFile = keyFile;
    m_databaseUserName = keyFile;
    m_databaseUserName.chop(4);
    qDebug() << "Loging in as" << m_databaseUserName;

    m_database->setHostName("127.0.0.1");
    m_database->setPort(3306);
    m_database->setDatabaseName(m_schema);
    m_database->setUserName(m_databaseUserName);
    m_database->setPassword(pass);

    setupTunnel();
}

void Database::readOutput()
{
    qDebug() << "[ssh] " << tunnelProcess->readAllStandardOutput();
}

void Database::readError()
{
    qDebug() << "[ssh] " << tunnelProcess->readAllStandardError();
}

void Database::openDatabaseConnection()
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

        emit changeStatus(tr("Połączenie z bazą danych %1 na %2 nie powiodło się.").arg(m_schema, m_host));
        emit connectionFail();
        return;
    }

    emit changeStatus(tr("Połączono z bazą danych %1 na %2").arg(m_schema, m_host));
    emit connectionSuccess();
}

void Database::failedTunnel(QProcess::ProcessError error) //nie dziala
{
    qDebug() << "ssh tunnel error:" << error;
    emit changeStatus(tr("Utworzenie tunelu do hosta %1 nie powiodło się.").arg(m_host));
    emit connectionFail();
}

void Database::setupTunnel()
{
    qDebug() << "setup ssh tunel";

    if(tunnelProcess != nullptr)
    {
        if(m_previousHost == m_host)
        {
            waitForTunnel();
            return;
        }
        else
        {
            tunnelProcess->kill();
            emit changeStatus(tr("Niszczenie tunelu do hosta %1...").arg(m_previousHost));
            tunnelProcess->waitForFinished(50000);
            delete tunnelProcess;
        }
    }

    m_previousHost = m_host;
    emit changeStatus(tr("Tworzenie tunelu do hosta %1...").arg(m_host));

    tunnelProcess = new QProcess(this);
    QString program;
    QStringList arguments;

#ifdef WIN32
    program = "./plink.exe";
    arguments << "-v" << "-ssh" << m_host << "-l" << m_databaseUserName << "-P" << "2292" << "-2" << "-4" << "-i" << m_keyFile << "-C" << "-T" << "-N" << "-L" << QString("3306:%1:3306").arg(m_host);
#else
    program = "ssh";
    arguments << m_host << "-p" << "2292" << "-l" << "konserw" << "-N" << "-L" << QString("3306:%1:3306").arg(m_host);
#endif

    connect(tunnelProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(tunnelProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
    connect(tunnelProcess, &QProcess::started, this, &Database::waitForTunnel);
    //connect(tunnelProcess, &QProcess::error, this, &Database::failedTunnel); //not working
    connect(tunnelProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(failedTunnel(QProcess::ProcessError)));

    tunnelProcess->start(program, arguments);
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

void Database::readSettings()
{
    QSettings settings;
    settings.beginGroup("connection");

    m_host = settings.value("selected host", "localhost").toString();
    if(settings.value("testDB").toBool())
        m_schema = "kOferta_test";
    else
        m_schema = "kOferta_v3";

    settings.endGroup();
}

void Database::waitForTunnel()
{
    QTcpSocket* sock = new QTcpSocket;
    sock->connectToHost("127.0.0.1", 3306);
    if(sock->waitForConnected(1000000))
    {
        emit changeStatus(tr("Tworzenie tunelu do hosta %1 zakończone powodzeniem").arg(m_host));
        qDebug() << "ssh tunnel opened to host " << m_host << " as " << m_databaseUserName;
        sock->disconnectFromHost();
        delete sock;
        openDatabaseConnection();
    }
    else
    {
	delete sock;
        qDebug() << "ssh: socket timeout";
        failedTunnel(QProcess::Timedout);
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

Database *Database::instance()
{
    if(m_instance == nullptr)
        m_instance = new Database;

    return m_instance;
}

User* Database::userInfo()
{
    qDebug() << "Downloading user info for" << m_databaseUserName;

    QSqlTableModel usersTable;
    usersTable.setTable("usersView");
    usersTable.setFilter(QString("dbName = '%1'").arg(m_databaseUserName));
    usersTable.select();
    qDebug() << "users table row count after filter:" << usersTable.rowCount();
    if(usersTable.rowCount() < 1)
        return nullptr;

    QSqlRecord r = usersTable.record(0);
    qDebug() << "user record" << r;
    return new User(r.value("uid").toInt(), r.value("name").toString(), r.value("phone").toString(), r.value("mail").toString(), r.value("address").toString(), r.value("male").toBool(), r.value("currentOfferNumber").toInt());
}

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

QString Database::mainAddress()
{
    QSqlTableModel model;
    model.setTable("address");
    model.setFilter(QString("id = 2"));
    model.select();

    QSqlRecord rec = model.record(0);
    return rec.value("address").toString();
}



