#include <QtTest/QtTest>
#include "Database.hpp"

class DatabaseTest: public QObject {
    Q_OBJECT

private:
    void setupConnection()
    {
        QString db_name = QProcessEnvironment::systemEnvironment().value("DATABASE", "kOferta_test");
        QString db_user = QProcessEnvironment::systemEnvironment().value("SQL_USER", "");
        QString db_pass = QProcessEnvironment::systemEnvironment().value("SQL_PASS", "");
        if(db_user.isEmpty() || db_pass.isEmpty())
            Database::instance()->setupDatabaseConnection("localhost", 3306, db_name);
        else
            Database::instance()->setupDatabaseConnection("localhost", 3306, db_name, db_user, db_pass);
    }
private slots:
    void initTestCase()
    {
        setupConnection();
        QVERIFY(Database::instance()->isConnected());
    }
    void cleanupTestCase()
    {
        Database::instance()->dropConection();
    }
    void firstTest()
    {
        QVERIFY(true);
    }
};


QTEST_MAIN(DatabaseTest)
#include "DatabaseTest.moc"
