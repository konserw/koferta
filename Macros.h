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


#ifdef WIN32
    extern QTextStream* logFile;
    #define OUTSTREAM *logFile << "\n"
#else
    #define OUTSTREAM qDebug()
#endif

#ifdef RELEASE
    #define DEBUG OUTSTREAM

#define EXEC_SILENT(s) EXEC(x)

    #define EXEC(x) \
        do{ \
            if(q.exec(s) == false) \
            { \
                DEBUG << "Zapytanie mysql zkończone niepowodzeniem!"; \
                OUTSTREAM  << "\tZapytanie mysql: " << s; \
                OUTSTREAM  << "\tError text: " <<  q.lastError().text(); \
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
    #define DEBUG OUTSTREAM << __FILE__ << " (" << __LINE__ << "): "

    #define EXEC(s) \
        do{ \
            DEBUG << "zapytanie mysql: " << s; \
            if(q.exec(s) == false) \
                { \
                DEBUG << "Zapytanie mysql zkończone niepowodzeniem!"; \
                OUTSTREAM << "\tError text: " <<  q.lastError().text(); \
                QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
                return; \
            } \
        }while(0)

    #define EXEC_SILENT(s) \
        do{ \
            if(q.exec(s) == false) \
            { \
                DEBUG << "Zapytanie mysql zkończone niepowodzeniem!"; \
                OUTSTREAM  << "\tZapytanie mysql: " << s; \
                OUTSTREAM  << "\tError text: " <<  q.lastError().text(); \
                QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
                return; \
            } \
        }while(0)

    #define LOGIN \
        if (!d->open()) \
        { \
            DEBUG << "Błąd: nie można się połączyć z bazą!"; \
            OUTSTREAM << "\t\t\t connName: " << d->connectionName(); \
            OUTSTREAM << "\t\t\t driver: " << d->driverName(); \
            OUTSTREAM << "\t\t\t opcje " << d->connectOptions(); \
            OUTSTREAM << "\t\t\t host: " << d->hostName(); \
            OUTSTREAM << "\t\t\t last error: " << d->lastError().text(); \
            QMessageBox::warning(NULL, "error", "Nie udało się nawiązać połączenia z bazą danych."); \
            return; \
        }

    #define LOGIN_ \
        if (!d->open()) \
        { \
            DEBUG << "Błąd: nie można się połączyć z bazą!"; \
            OUTSTREAM << "\t\t\t connName: " << d->connectionName(); \
            OUTSTREAM << "\t\t\t driver: " << d->driverName(); \
            OUTSTREAM << "\t\t\t opcje " << d->connectOptions(); \
            OUTSTREAM << "\t\t\t host: " << d->hostName(); \
            OUTSTREAM << "\t\t\t last error: " << d->lastError().text(); \
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
