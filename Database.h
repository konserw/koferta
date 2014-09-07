#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include "TermModel.h"

class QString;
class User;
class QSqlTableModel;
class Merchandise;

class Database : public QObject
{
Q_OBJECT

public:
    enum TermType
    {
        termShipping = 0,//dostawa
        termOffer = 1,   //oferta
        termPayment = 2, //platnosc
        termShipmentTime = 3 //termin
    };

/* Database Interface */
    //user-related
    static QStringList usersList();
    static User userInfo(const QString& name);

    //terms
    static void createTerms(TermType type, const QString& shortDesc, const QString& longDesc);

    static TermModel *paymentTermsModel();
    static TermModel *shippingTermsModel();
    static TermModel *shipmentTimeModel();
    static TermModel *offerTermsModel();

    static TermItem paymentTerm(int id);
    static TermItem shippingTerm(int id);
    static TermItem shipmentTime(int id);
    static TermItem offerTerm(int id);

    static void saveOfferMerchandise(const QString& offerId, const QList<Merchandise*>& merchandise);


/* Database connection setup */
    explicit Database(QObject *parent = NULL);
    ~Database();

public slots:
    void connect(const QString &uid, const QString& pass);
    void getUsersList();

signals:
    void newUsers(const QStringList&);
    void connectionSuccess(const User&);
    void changeStatus(const QString&);

protected:
    ///Database object
    QSqlDatabase* m_database;

    void setupConnection(const QString &user, const QString pass = QString::null);

    inline void setupSSL();
    inline bool openDatabaseConnection();

    static TermModel* getTermModel(TermType termType);
    static TermItem getTerm(TermType termType, int id = -1);

    QString m_host;
    QString m_schema;
    bool m_sslEnabled;

};

void insert_klient(const QString &skrot, const QString &full, const QString &tytul, const QString &imie, const QString &nazwisko, const QString &adres);
void insert_zapisane(const QString& nr_oferty, int id_klienta, const QString& data, int uid, const QString& zapytanie_data, const QString& zapytanie_nr, int dostawa, int termin, int platnosc, int oferta, const QString& uwagi);
void insert_combo(const QString& typ, const QString& sh, const QString& lo);

#endif // DATABASE_H
