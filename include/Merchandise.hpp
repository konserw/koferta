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

#ifndef TOWAR_H
#define TOWAR_H

#include <QString>
#include "MainwindowExport.hpp"

/*!
 * \brief Klasa reprezentująca towar w ofercie
 */
class MAINWINDOW_EXPORT Merchandise
{
public:
    explicit Merchandise();
    explicit Merchandise(int id);
    Merchandise(int id, const QString& kod, const QString& nazwa, double cena, bool metr = false, int ilosc = 0);
    
    int id() const;
    void setId(int id);

    QString kod() const;
    void setKod(const QString &kod);

    QString nazwa() const;
    void setNazwa(const QString &nazwa);

    double cenaKat() const;
    void setCenaKat(double cenaKat);
    double cenaKat(double kurs) const;

    double rabat() const;
    void setRabat(double rabat);

    double ilosc() const;
    void setIlosc(double ilosc);

    bool metr() const;
    void setMetr(bool metr);
    QString unit() const;

    double cena() const;
    double cena(double kurs) const;

    double wartosc() const;  
    double wartosc(double kurs) const;

    bool operator==(const Merchandise& other) const;

    static double roundTo2(double val);

protected:
    int     m_id;
    QString m_kod;
    QString m_nazwa;
    double  m_cenaKat;
    bool    m_metr;

    double  m_rabat;
    double  m_ilosc;    
};

#endif // TOWAR_H
