#ifndef TOWAR_H
#define TOWAR_H

#include <QString>

/*!
 * \brief Klasa reprezentująca towar w ofercie
 */
class Merchandise
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
