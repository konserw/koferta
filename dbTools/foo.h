#ifndef FOO_H
#define FOO_H

#include <QDebug>

#define DEBUG qDebug() << __FILE__ << " (" << __LINE__ << "): "

#define EXEC(s) \
    do{ \
        DEBUG << "zapytanie mysql: " << s; \
        if(q.exec(s) == false) \
        { \
            DEBUG << "Zapytanie mysql zkończone niepowodzeniem!"; \
            qDebug() << "\tError text: " <<  q.lastError().driverText(); \
            return; \
        } \
    }while(0)

#define EXEC_SILENT(s) \
    do{ \
        if(q.exec(s) == false) \
        { \
            DEBUG << "Zapytanie mysql zkończone niepowodzeniem!"; \
            qDebug() << "\tZapytanie mysql: " << s; \
            qDebug() << "\tError text: " <<  q.lastError().text(); \
            return; \
        } \
    }while(0)

void insert_towar(const QString& id, const QString& nazwa, double cena, const QString& jednostka, bool checkExisting = true);
QStringList importTowar(const QString& fileName, bool dryRun = false);

#endif // FOO_H
