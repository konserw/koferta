#ifndef MACROS_H
#define MACROS_H

#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QTextStream>

#ifdef RELEASE
    #define DEBUG QTextStream(stdout)

    #define EXEC(x) \
        do{ \
            if(q.exec(s) == false) \
                { \
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
            s += *host; \
            s += " nie powiodło się."; \
            ui->info->setText(s); \
            return; \
        } \
        else \
        { \
            s = "Połączono z bazą danych na "; \
            s += *host; \
            ui->info->setText(s); \
        }
#else

    #define DEBUG qDebug() << __FILE__ << " (" << __LINE__ << "): "

    #define EXEC(s) \
        do{ \
            qDebug() << __FILE__ << " (" << __LINE__ << "): zapytanie mysql: " << s; \
            if(q.exec(s) == false) \
                { \
                qDebug() << __FILE__ << " (" << __LINE__ << "): Zapytanie mysql zkończone niepowodzeniem!"; \
                QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
                return; \
            } \
        }while(0)

    #define LOGIN \
        if (!d->open()) \
        { \
            qDebug() << __FILE__ << " (" << __LINE__ << "): Błąd: nie można się połączyć z bazą!"; \
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
            qDebug() << __FILE__ << " (" << __LINE__ << "): Błąd: nie można się połączyć z bazą!"; \
            qDebug() << "\t\t\t connName: " << d->connectionName(); \
            qDebug() << "\t\t\t driver: " << d->driverName(); \
            qDebug() << "\t\t\t opcje " << d->connectOptions(); \
            qDebug() << "\t\t\t host: " << d->hostName(); \
            qDebug() << "\t\t\t last error: " << d->lastError().text(); \
            s = "Połączenie z bazą danych na "; \
            s += *host; \
            s += " nie powiodło się."; \
            ui->info->setText(s); \
            return; \
        } \
        else \
        { \
            s = "Połączono z bazą danych na "; \
            s += *host; \
            ui->info->setText(s); \
        }
#endif // RELEASE

#endif // MACROS_H
