#include <QSqlQuery>
#include <QString>
#include "Database.h"
#include "Macros.h"

void insert_towar(QString id, QString nazwa, QString cena, QString jednostka)
{
    QSqlQuery q;
    QString s;
    s = "SELECT nazwa FROM towar WHERE id='";
    s += id;
    s += "'";
    EXEC(s);
    if(q.next())
    {
        DEBUG <<  "Towar o id: " << id << " istnieje w bazie, nastapi jego nadpisanie!";
        s = "DELETE FROM towar WHERE id='";
        s += id;
        s += "'";

        EXEC(s);
    }
    s = "INSERT INTO towar (id, nazwa, cena_kat, jednostka) VALUES ('";
    s += id;
    s += "', '";
    s += nazwa;
    s += "', '";
    s += cena;
    s += "', '";
    s += jednostka;
    s += "');";
    DEBUG <<  "Wstawianie towaru, id: " << id;

    EXEC(s);
}

void insert_klient(QString skrot, QString full, QString tytul, QString imie, QString nazwisko, QString adres)
{
    QString s;
    QSqlQuery q;
    s = "INSERT INTO klient (short, full, tytul, imie, nazwisko, adres) VALUES ('";
    s += skrot;
    s += "', '";
    s += full;
    s += "', '";
    s += tytul;
    s += "', '";
    s += imie;
    s += "', '";
    s += nazwisko;
    s += "', '";
    s += adres;
    s += "');";
    DEBUG <<  "Wstawianie klienta, nazwisko: " << nazwisko;

    EXEC(s);
}

void insert_zapisane(QString nr_oferty, QString id_klienta, QString data, QString uid, QString zapytanie, QString dostawa, QString termin, QString platnosc, QString oferta)
{
    QString s;
    QSqlQuery q;
    s = "DELETE FROM zapisane WHERE nr_oferty = '";
    s += nr_oferty;
    s += "'";
    EXEC(s);

    s = "DELETE FROM zapisane_towary WHERE nr_oferty = '";
    s += nr_oferty;
    s += "'";
    EXEC(s);

    s = "INSERT INTO zapisane "
        "(nr_oferty, id_klienta, data, uid, zapytanie, dostawa, termin, platnosc, oferta)"
        " VALUES ('";
    s += nr_oferty;
    s += "', ";
    s += id_klienta;
    s += ", '";
    s += data;
    s += "', ";
    s += uid;
    s += ", '";
    s += zapytanie;
    s += "', '";
    s += dostawa;
    s += "', '";
    s += termin;
    s += "', '";
    s += platnosc;
    s += "', '";
    s += oferta;
    s += "')";
    DEBUG <<  "Zapisywanie ofery nr: " << nr_oferty;

    EXEC(s);
}

void insert_zapisane_towary(QString nr_oferty, QString kod, QString ilosc, QString rabat)
{
    QString s;
    QSqlQuery q;
    s = "INSERT INTO zapisane_towary (nr_oferty, kod, ilosc, rabat) VALUES ('";
    s += nr_oferty;
    s += "', '";
    s += kod;
    s += "', ";
    s += ilosc;
    s += ", ";
    s += rabat;
    s += ")";
    DEBUG <<  "Zapisywanie oferty nr: " << nr_oferty << " - kod: " << kod;

    EXEC(s);
}
