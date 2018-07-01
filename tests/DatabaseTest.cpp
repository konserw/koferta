#include <QtTest/QtTest>
#include "Database.hpp"
namespace tests {

class database : public QObject {
    Q_OBJECT
private slots:
    void setup_connection()
    {
        QString db_name = QProcessEnvironment::systemEnvironment().value("DATABASE", "kOferta_test");
        QString db_user = QProcessEnvironment::systemEnvironment().value("SQL_USER", "");
        QString db_pass = QProcessEnvironment::systemEnvironment().value("SQL_PASS", "");
        if(db_user.isEmpty() || db_pass.isEmpty())
            Database::instance()->setupDatabaseConnection("localhost", 3306, db_name);
        else
            Database::instance()->setupDatabaseConnection("localhost", 3306, db_name, db_user, db_pass);
        QVERIFY(Database::instance()->isConnected());
        Database::instance()->dropConection();
    }
};

}

QTEST_MAIN(tests::database)
#include "DatabaseTest.moc"
