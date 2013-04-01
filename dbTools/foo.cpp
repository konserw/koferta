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

    s = QString("INSERT INTO towar (id, nazwa, cena, jednostka) VALUES ('%1', '%2', %3, '%4')").arg(id, nazwa, QString::number(cena), jednostka);
    EXEC_SILENT(s);
}


QStringList importTowar(const QString& fileName, bool dryRun)
{
    static QStringList keys;
    QString key;
    QStringList list;
    QString sRead;
    QString cena;
    QDebug dbg(QtDebugMsg);
    QStringList sl;
    QString s;
    double dCena;
    bool f;
    QSqlQuery q;
    QVariantList ids, nazwy, ceny, j;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if(!file.exists())DEBUG <<  "cennik nie iesnieje";
        else DEBUG <<  "cennik niedostępny";
        return QStringList() << fileName << " niedostępny";
    }

    qDebug() << "\n#############################################################################";
    qDebug() <<  "#\tPrzetwarzanie pliku:" << fileName << "\t#";
    qDebug() << "#############################################################################\n";

    QTextStream in(&file);
    in.setCodec("UTF-8");

    for(unsigned i=0; !in.atEnd(); ++i)
    {
        sRead = in.readLine();
        list = sRead.split("|");
        if(list.size() < 3)
        {
            sl << QString("%1\t| %2\tZa mało kolumn").arg(fileName, QString::number(i+1));
            continue;
        }

        key = list[1];
        if(keys.contains(key))
        {
            sl << QString("%1\t| %2\tDuplikat klucza: %3").arg(fileName, QString::number(i+1), key);
            continue;
        }
        keys << key;

        cena =  list[2];
        cena.remove(' ');
        cena.replace(',', '.');
        dCena = cena.toDouble(&f);
        if(!f)
        {
            sl << QString("%1\t| %2\tZła cena: %3").arg(fileName, QString::number(i+1), cena);
            continue;
        }

        if(list.size() > 3 && (list[3] == "mb." || list[3] == "m" || list[3] == "metr"))
            s = "mb.";
        else
            s = "szt.";

     //   if(1%50 == 0)
            dbg <<  "\rPrzetworzono lini:\t" << i+1;

        if(!dryRun)
        {
            nazwy << list[0];
            ceny << dCena;
            j << s;
        }
    }

    dbg << "\n";

    if(dryRun)
        return sl;

    q.prepare("INSERT INTO towar (id, nazwa, cena, jednostka) VALUES (:id, :nazwa, :cena, :j) ON DUPLICATE KEY UPDATE nazwa=VALUES(nazwa), cena=VALUES(cena), jednostka=VALUES(jednostka)");
    q.bindValue(":id", ids);
    q.bindValue(":nazwa", nazwy);
    q.bindValue(":cena", ceny);
    q.bindValue(":j", j);

    if (!q.execBatch())
         DEBUG << q.lastError();

    return sl;
}
