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
#include <QProcess>
#include "TermModel.h"

class QString;
class User;
class QSqlTableModel;
class Merchandise;
class LoadDialogMerchandiseListModel;
class QProcess;
class QTcpSocket;
class QProgressDialog;

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

    static Database* instance();

/* Database Interface */
    //user-related
    QStringList usersList();
    User *userInfo();
    bool setCurrentOfferNumber(int offerNumber);

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

    //merchandise
    static void saveOfferMerchandise(const QString& offerId, const QList<Merchandise*>& merchandise);
    static QList<Merchandise*> loadOfferMerchandise(const QString& number);
    static LoadDialogMerchandiseListModel* loadDialogMerchandiseListModel(QObject* parent);

    //other
    static QString mainAddress();

public slots:
    void dropConection();
    void waitForTunnel();
    void tunelOpened();
    void setupDatabaseConnection(const QString &keyFile, const QString& pass);
    void readOutput();
    void readError();
    void tunnelCancel();
    void openDatabaseConnection();
    void failedTunnel(QProcess::ProcessError error);

    void socketConnected();
signals:
    void driverFail();
    void connectionFail();
    void connectionSuccess();
    void changeStatus(const QString&);

protected:
    explicit Database(QObject *parent = nullptr);
    ~Database();
    static Database* m_instance;

    void setupTunnel();
    void createMysqlDatabase();
    void readSettings();

    static TermModel* getTermModel(TermType termType);
    static TermItem getTerm(TermType termType, int id = -1);

    QString m_databaseUserName;
    QString m_keyFile;
    QString m_host;
    static const int m_port = 2292;
    static const int m_localPort = 13306;
    static const int m_hostPort = 3306;
    QString m_previousHost;
    QString m_schema;

    QProgressDialog* m_progressDialog;
    QTcpSocket* m_socket;
    QProcess *tunnelProcess;
    QSqlDatabase* m_database;
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
