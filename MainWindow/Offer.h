#ifndef OFFER_H
#define OFFER_H

#include "TermItem.h"

#include <QObject>
#include <QString>

class MerchandiseListModel;
class QTextDocument;
class QPrinter;

class Offer : public QObject
{
    Q_OBJECT
public:
    explicit Offer(QObject *parent = 0);

    TermItem getShippingTerm() const;
    TermItem getShipmentTime() const;
    TermItem getPaymentTerm() const;
    TermItem getOfferTerm() const;

    ///"drukowanie" dokumentu do podglądu lub pdf
    void print(QPrinter *printer);
    QTextDocument document() const;

signals:

public slots:
    void setOfferTerm(const TermItem& term);
    void setPaymentTerm(const TermItem& term);
    void setShippingTerm(const TermItem& term);
    void setShipmentTime(const TermItem& term);

protected:
    int number;
    QString numberWithYear;
    QString date;
    double exchangeRate;
    bool pln;

    MerchandiseListModel* merchandiseList;

    TermItem shippingTerm;
    TermItem shipmentTime;
    TermItem offerTerm;
    TermItem paymentTerm;
};

#endif // OFFER_H
