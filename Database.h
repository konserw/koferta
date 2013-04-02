#ifndef DATABASE_H
#define DATABASE_H
class QString;

void insert_towar(const QString &id, const QString &nazwa, double cena, const QString &jednostka);
void insert_klient(const QString &skrot, const QString &full, const QString &tytul, const QString &imie, const QString &nazwisko, const QString &adres);
void insert_zapisane_towary(const QString &nr_oferty, const QString &kod, double ilosc, double rabat);
void insert_zapisane(const QString& nr_oferty, int id_klienta, const QString& data, int uid, const QString& zapytanie_data, const QString& zapytanie_nr, int dostawa, int termin, int platnosc, int oferta, const QString& uwagi);
void insert_combo(const QString& typ, const QString& sh, const QString& lo);

#endif // DATABASE_H
