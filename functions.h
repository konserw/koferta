#ifndef FUNCTIONS_H
#define FUNCTIONS_H
class QString;

#include <QString>
#include <QCoreApplication>
#include <QDir>

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
