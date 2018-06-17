#include <QtTest/QtTest>
#include "Database.hpp"
namespace tests {

class database : public QObject {
    Q_OBJECT
private slots:
    void setup_connection()
    {
        QString db_name = QProcessEnvironment::systemEnvironment().value("DATABASE_MYSQL_USERNAME", "test_user");
        QString db_pass = QProcessEnvironment::systemEnvironment().value("DATABASE_MYSQL_PASSWORD", "test_password");
        Database::instance()->setupDatabaseConnection("localhost", 3306, "kOferta_test", db_name, db_pass);
        QVERIFY(Database::instance()->isConnected());
        Database::instance()->dropConection();
    }
};

}

QTEST_MAIN(tests::database)
#include "database.moc"
