#ifndef MACROS_H
#define MACROS_H

#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QTextStream>
#include <QSqlQuery>
#include <QFile>

#define EXEC_SILENT(s) \
do{ \
    if(q.exec(s) == false) \
    { \
        qDebug() << "Zapytanie mysql zkończone niepowodzeniem!"; \
        qDebug()  << "\tZapytanie mysql: " << s; \
        qDebug()  << "\tError text: " <<  q.lastError().text(); \
        QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
        return; \
    } \
}while(0)

#endif // MACROS_H
