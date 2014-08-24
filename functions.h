#ifndef FUNCTIONS_H
#define FUNCTIONS_H
class QString;

#include <QString>
#include <QCoreApplication>
#include <QDir>

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

/*!
 * \brief filePath wyznacza bezwzględną ścieżka do pliku o nazwie jak plik wykonywalny z rozszerzeniem podanym jako parametr
 * \param suffix Rozszerzenie pliku (podawać z kropką)
 * \return QString zawierający ścieżkę bezwzględą z natywnymi separatorami
 */
inline QString filePath(const char* suffix)
{
    QString path = QCoreApplication::applicationFilePath();
#ifdef WIN32
    path = path.replace(".exe", suffix);
#else
    path = path + suffix;
#endif
    return QDir::toNativeSeparators(path);
}

#endif // FUNCTIONS_H
