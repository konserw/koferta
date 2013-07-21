#ifndef TOWAR_H
#define TOWAR_H

#include <QObject>

class Towar : public QObject
{
    Q_OBJECT
public:
    explicit Towar(QObject *parent = 0);
    
signals:
    void update();

public slots:
/*
    void setIlosc(double x);
    void setRabat(double x);

    double cena();
    double wartosc();
    double cenaPln();

protected:
    QString m_kod;
    QString m_nazwa;
    double m_cenaKat;
    double m_rabat;
    double m_ilosc;
    bool m_metr;
*/
};

#endif // TOWAR_H
