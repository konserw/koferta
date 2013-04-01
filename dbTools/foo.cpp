#include "foo.h"
#include <QtSql>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

void insert_towar(const QString& id, const QString& nazwa, double cena, const QString& jednostka, bool checkExisting)
{
    QSqlQuery q;
    QString s;

    if(checkExisting)
    {
        s = QString("SELECT nazwa FROM towar WHERE id='%1'").arg(id);
        EXEC_SILENT(s);
        if(q.next())
        {
            DEBUG <<  "Towar o id: " << id << " istnieje w bazie, nastapi jego nadpisanie!";
            s = QString("DELETE FROM towar WHERE id='%1'").arg(id);
            EXEC_SILENT(s);
        }
    }

    s = QString("INSERT INTO towar (id, nazwa, cena, jednostka) VALUES ('%1', '%2', '%3', '%4')").arg(id, nazwa, QString::number(cena), jednostka);
    EXEC_SILENT(s);
}


QStringList importTowar(const QString& fileName, bool dryRun, bool checkExisting)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if(!file.exists())DEBUG <<  "cennik nie iesnieje";
        else DEBUG <<  "cennik niedostępny";
        return QStringList() << fileName << " niedostępny";
    }

    qDebug() << "\n############################################";
    qDebug() <<  "#\tPrzetwarzanie pliku:" << fileName << "\t#";
    qDebug() << "############################################";

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QStringList list;
    QString sRead;
    QString cena;
    QDebug dbg(QtDebugMsg);
    QStringList sl;
    QString s;

    for(unsigned i=0; !in.atEnd(); ++i)
    {
        sRead = in.readLine();
        list = sRead.split("|");
        if(list.size() < 3)
        {
            sl << QString("%1\t| %2").arg(fileName, QString::number(i+1));
            continue;
        }

        if(dryRun)
        {
            dbg <<  "Przetworzono lini:\t" << i+1 << "           \r";
            continue;
        }

        if(list.size() > 3 && (list.at(3) == "mb." || list.at(3) == "m" || list.at(3) == "metr"))
            s = "mb.";
        else
            s = "szt.";

        cena =  list.at(2);
        cena.remove(' ');
        cena.replace(',', '.');

        dbg <<  "Wstawianie towaru, id:\t" << list.at(1) << "                 \r";
        insert_towar(list.at(1), list.at(0), cena.toDouble(), s, checkExisting);
    }

    return sl;
}
