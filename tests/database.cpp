#include <QtTest/QtTest>
#include "Database.hpp"
namespace tests {

class database : public QObject {
    Q_OBJECT
private slots:
    void setup_connection()
    {
        QString db_name = QProcessEnvironment::systemEnvironment().value("DATABASE", "kOferta_test");
        Database::instance()->setupDatabaseConnection("localhost", 3306, db_name);
        QVERIFY(Database::instance()->isConnected());
        Database::instance()->dropConection();
    }
};

}

QTEST_MAIN(tests::database)
#include "database.moc"
