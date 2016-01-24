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

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMessageBox>
#include "TermModel.h"

class QString;
class User;
class QSqlTableModel;
class Merchandise;
class LoadDialogMerchandiseListModel;

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
    static QList<Merchandise*> loadOfferMerchandise(const QString& number);

    static LoadDialogMerchandiseListModel* loadDialogMerchandiseListModel(QObject* parent);

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

#define EXEC_SILENT(s) \
do{ \
    if(q.exec(s) == false) \
    { \
        qDebug() << "Zapytanie mysql zkończone niepowodzeniem!"; \
        qDebug()  << "\tZapytanie mysql: " << s; \
        qDebug()  << "\tError text: " <<  q.lastError().text(); \
        QMessageBox::warning(NULL, "error", "Wystąpił błąd połączenia z bazą danych. Sprawdź połączenie i spróbuj ponownie"); \
        return; \
    } \
}while(0)

#endif // DATABASE_H
