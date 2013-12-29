#include "Merchandise.h"
#include <QSqlRecord>
#include <QVariant>

Merchandise::Merchandise() :
    m_rabat(0), m_ilosc(0)
{   
}

Merchandise::Merchandise(int id) :
    m_id(id)
{
}

Merchandise::Merchandise(int id, const QString &kod, const QString &nazwa, double cena, bool metr, int ilosc) :
    m_id(id), m_kod(kod), m_nazwa(nazwa), m_cenaKat(cena), m_metr(metr), m_rabat(0), m_ilosc(ilosc)
{
}

double Merchandise::cenaPln(double kurs) const
{
    return m_cenaKat * kurs;
}

QString Merchandise::kod() const
{
    return m_kod;
}

void Merchandise::setKod(const QString &kod)
{
    m_kod = kod;
}

QString Merchandise::nazwa() const
{
    return m_nazwa;
}

void Merchandise::setNazwa(const QString &nazwa)
{
    m_nazwa = nazwa;
}

double Merchandise::cenaKat() const
{
    return m_cenaKat;
}

void Merchandise::setCenaKat(double cenaKat)
{
    m_cenaKat = cenaKat;
}

double Merchandise::rabat() const
{
    return m_rabat;
}

void Merchandise::setRabat(double rabat)
{
    m_rabat = rabat;
}

double Merchandise::ilosc() const
{
    return m_ilosc;
}

void Merchandise::setIlosc(double ilosc)
{
    m_ilosc = ilosc;
}

bool Merchandise::metr() const
{
    return m_metr;
}

void Merchandise::setMetr(bool metr)
{
    m_metr = metr;
}

double Merchandise::cena() const
{
    return m_cenaKat * (100 - m_rabat) /100;
}

double Merchandise::wartosc() const
{
    return m_ilosc * cena();
}

double Merchandise::cena(double kurs) const
{
    return cenaPln(kurs) * (100 - m_rabat) /100;
}

double Merchandise::wartosc(double kurs) const
{
    return m_ilosc * cena(kurs);
}
int Merchandise::id() const
{
    return m_id;
}

void Merchandise::setId(int id)
{
    m_id = id;
}

bool Merchandise::operator==(const Merchandise &other) const
{
    return other.m_id == m_id;
}


