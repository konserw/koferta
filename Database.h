#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

class QSqlDatabase;
class QString;
class cUser;
class QSqlTableModel;

class Database : public QObject
{
Q_OBJECT

public:
    explicit Database(QObject *parent = NULL);
    ~Database();

public slots:
    void hostChanged(QString ip);
    void connect(const QString &uid, const QString& pass);

signals:
    void newUsers(const QStringList&);
    void connectionSuccess(const cUser&);
    void changeStatus(const QString&);

protected:
    ///Database object
    QSqlDatabase* m_initialConnection;
    QSqlTableModel* m_usersTable;

    inline void init(QSqlDatabase &db);
    inline void login(QSqlDatabase &db);
};


void insert_towar(const QString &id, const QString &nazwa, double cena, const QString &jednostka);
void insert_klient(const QString &skrot, const QString &full, const QString &tytul, const QString &imie, const QString &nazwisko, const QString &adres);
void insert_zapisane_towary(const QString &nr_oferty, const QString &kod, double ilosc, double rabat);
void insert_zapisane(const QString& nr_oferty, int id_klienta, const QString& data, int uid, const QString& zapytanie_data, const QString& zapytanie_nr, int dostawa, int termin, int platnosc, int oferta, const QString& uwagi);
void insert_combo(const QString& typ, const QString& sh, const QString& lo);

//QStringList getUsersList();
#endif // DATABASE_H
