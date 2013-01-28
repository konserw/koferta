#ifndef DATABASE_H
#define DATABASE_H
class QString;

void insert_towar(QString id, QString nazwa, QString cena, QString jednostka);
void insert_klient(QString skrot, QString full, QString tytul, QString imie, QString nazwisko, QString adres);
void insert_zapisane_towary(QString nr_oferty, QString kod, QString ilosc, QString rabat);
void insert_zapisane(QString nr_oferty, int id_klienta, QString data, QString uid, QString zapytanie, QString dostawa, QString termin, QString platnosc, QString oferta);

#endif // DATABASE_H
