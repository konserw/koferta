#ifndef MACROS_H
#define MACROS_H

#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QTextStream>
#include <QSqlQuery>
#include <QFile>

class cUser;
extern cUser* currentUser;

#ifdef RELEASE
    #define DEBUG qDebug()

#define EXEC_SILENT(s) EXEC(x)

    #define EXEC(x) \
        do{ \
            if(q.exec(s) == false) \
            { \
                DEBUG << "Zapytanie mysql zkonczone niepowodzeniem!"; \
                qDebug()  << "\tZapytanie mysql: " << s; \
                qDebug()  << "\tError text: " <<  q.lastError().text(); \
                QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
                return; \
            } \
        }while(0)

    #define LOGIN \
        if(!d->open()) \
        { \
            QMessageBox::warning(NULL, "error", "Nie udało się nawiązać połączenia z bazą danych."); \
            return; \
        }

    #define LOGIN_ \
        if (!d->open()) \
        { \
            s = "Połączenie z bazą danych na "; \
            s += d->hostName(); \
            s += " nie powiodło się."; \
            ui->info->setText(s); \
            return; \
        } \
        else \
        { \
            s = "Połączono z bazą danych na "; \
            s += d->hostName(); \
            ui->info->setText(s); \
        }

#else
inline QDebug debug()
{
    return qDebug() << __FILE__ << " (" << __LINE__ << "): ";
}
//#define DEBUG debug()
    #define DEBUG qDebug() << __FILE__ << " (" << __LINE__ << "): "

    #define EXEC(s) \
        do{ \
            DEBUG << "zapytanie mysql: " << s; \
            if(q.exec(s) == false) \
                { \
                DEBUG << "Zapytanie mysql zkonczone niepowodzeniem!"; \
                qDebug() << "\tError text: " <<  q.lastError().text(); \
                QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
                return; \
            } \
        }while(0)

    #define EXEC_SILENT(s) \
        do{ \
            if(q.exec(s) == false) \
            { \
                DEBUG << "Zapytanie mysql zkończone niepowodzeniem!"; \
                qDebug()  << "\tZapytanie mysql: " << s; \
                qDebug()  << "\tError text: " <<  q.lastError().text(); \
                QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
                return; \
            } \
        }while(0)

    #define LOGIN \
        if (!d->open()) \
        { \
            DEBUG << "Błąd: nie można się połączyć z bazą!"; \
            qDebug() << "\t\t\t connName: " << d->connectionName(); \
            qDebug() << "\t\t\t driver: " << d->driverName(); \
            qDebug() << "\t\t\t opcje " << d->connectOptions(); \
            qDebug() << "\t\t\t host: " << d->hostName(); \
            qDebug() << "\t\t\t last error: " << d->lastError().text(); \
            QMessageBox::warning(NULL, "error", "Nie udało się nawiązać połączenia z bazą danych."); \
            return; \
        }

    #define LOGIN_ \
        if (!d->open()) \
        { \
            DEBUG << "Błąd: nie można się połączyć z bazą!"; \
            qDebug() << "\t\t\t connName: " << d->connectionName(); \
            qDebug() << "\t\t\t driver: " << d->driverName(); \
            qDebug() << "\t\t\t opcje " << d->connectOptions(); \
            qDebug() << "\t\t\t host: " << d->hostName(); \
            qDebug() << "\t\t\t last error: " << d->lastError().text(); \
            s = "Połączenie z bazą danych na "; \
            s += d->hostName(); \
            s += " nie powiodło się."; \
            ui->info->setText(s); \
            return; \
        } \
        else \
        { \
            s = "Połączono z bazą danych na "; \
            s += d->hostName(); \
            ui->info->setText(s); \
        }
#endif // RELEASE

#endif // MACROS_H
