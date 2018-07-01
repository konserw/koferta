/**
    kOferta - system usprawniajacy proces ofertowania
    Copyright (C) 2011  Kamil 'konserw' Strzempowicz, konserw@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "Merchandise.hpp"
#include <QSqlRecord>
#include <QVariant>
#include <cmath>

double Merchandise::roundTo2(double val)
{
    return llround(val * 100) / 100.0f;
}

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

double Merchandise::cenaKat(double kurs) const
{
    return roundTo2(m_cenaKat * kurs);
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
    m_cenaKat = roundTo2(cenaKat);
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

QString Merchandise::unit() const
{
    if(m_metr)
        return QObject::tr("mb.");
    else
        return QObject::tr("szt.");
}

double Merchandise::cena() const
{
    return roundTo2(m_cenaKat * (100 - m_rabat) /100);
}

double Merchandise::wartosc() const
{
    return roundTo2(m_ilosc * cena());
}

double Merchandise::cena(double kurs) const
{
    return roundTo2(cenaKat(kurs) * (100 - m_rabat) /100);
}

double Merchandise::wartosc(double kurs) const
{
    return roundTo2(m_ilosc * cena(kurs));
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

