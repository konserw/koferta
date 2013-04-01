#include <QtCore/QCoreApplication>
#include <mysql/mysql.h>
#include <QString>
#include <QTextCodec>
#include <QtSql/QtSql>
#include <QStringList>
#include "foo.h"
#include <unistd.h>
#include <QDirIterator>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QStringList ar = a.arguments();
    if(ar.size() < 2 || ar.contains("-h") || ar.contains("--help") || ar.contains("/?"))
    {
        qDebug() << "dbTools [-h] [-d] [-r path] [file1.csv ...]";
        qDebug() << "-h --help\tPrint this help and exit";
        qDebug() << "-d\t\tDry run - check syntax, not inserting into base";
        qDebug() << "-r path\t\tRecursive - scans path for all .csv files";
        return 0;
    }

    ar.removeFirst();

    bool dryRun = false;
    bool recursive = false;

    if(ar.contains("-d"))
        dryRun = true;
    if(ar.contains("-r"))
        recursive = true;

    if(!dryRun)
    {
        const QString ip = "192.168.1.90";
        QString pp;
        pp = "Enter root password for MySql database on ";
        pp += ip;
        pp += ":\n";
        char* cPass = getpass(pp.toStdString().c_str());
        QString pass(cPass);
        if(pass.isEmpty())
        {
            DEBUG << "no password!";
            return 0;
        }

        QSqlDatabase* d = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
        d->setHostName(ip);
        d->setPort(3306);
        d->setConnectOptions("CLIENT_SSL=1;CLIENT_IGNORE_SPACE=1");
        d->setUserName("root");
        d->setPassword(pass);
        d->setDatabaseName("kOferta");

        QVariant v = d->driver()->handle();
        if (v.isValid() && qstrcmp(v.typeName(), "MYSQL*")==0)
        {
            MYSQL *handle = static_cast<MYSQL *>(v.data());
            if (handle != NULL)
            {
                mysql_ssl_set(handle, ":/client-key",
                    ":/client-cert", ":/ca-cacert",
                    NULL, NULL);
            }
        }
        else
        {
            QStringList list;
            DEBUG << "invalid driver\n"
             << "library paths: ";
            list = qApp->libraryPaths();
            for(int i=0; i<list.size(); i++)
                qDebug() << "\t\t\t" << list[i];

            DEBUG << "aviable drivers: ";
            list = QSqlDatabase::drivers();
            for(int i=0; i<list.size(); i++)
                qDebug() << "\t\t\t" << list[i];
            return 1;
        }

        if (!d->open())
        {
            DEBUG << "Błąd: nie można się połączyć z bazą!";
            qDebug() << "\t\t\t connName: " << d->connectionName();
            qDebug() << "\t\t\t driver: " << d->driverName();
            qDebug() << "\t\t\t opcje " << d->connectOptions();
            qDebug() << "\t\t\t host: " << d->hostName();
            qDebug() << "\t\t\t last error: " << d->lastError().text();
            return 2;
        }
    }

    QString s;
    QStringList sl;
    QString path;
    int files=0;

    if(recursive)
    {
        foreach(s, ar)
        {
           if(s[0] == '-')
               continue;
           path = s;
           break;
        }
        QDirIterator it(path, QDirIterator::NoIteratorFlags);
        while (it.hasNext())
        {
            s = it.next();
            if(s.contains(".csv"))
            {
                files++;
                sl += importTowar(s, dryRun);
            }
        }

    }
    else
    {
        foreach(s, ar)
        {
            if(s[0] == '-')
                continue;
            files++;
            sl += importTowar(s, dryRun);
        }
    }

    qDebug() << "\n##############################################";
    qDebug() << "Zakończono pracę. Błedne wpisy w plikach:";
    foreach(s, sl)
        qDebug() << s;

    qDebug() << "Przetworzono plików: " << files;
    return 0;
}
