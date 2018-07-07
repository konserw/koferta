#include <QtTest/QtTest>
#include "Database.hpp"
#include "DatabaseHelpers.hpp"
#include "TermItem.hpp"

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
    void createTermTest()
    {
        QHash<TermType, QString> termTable
        {
            {TermType::billing, "billingTerms"},
            {TermType::delivery, "deliveryTerms"},
            {TermType::deliveryDate, "deliveryDateTerms"},
            {TermType::offer, "offerTerms"}
        };
        TermItem testTerm(TermType::billing, "billing short", "billing long");
        Database::createTerm(testTerm);

        QString queryText;
        queryText = QString("SELECT id, shortDesc, longDesc FROM %1")
                .arg(termTable[testTerm.getType()]);

        Transaction::open();
        auto query = Transaction::run(queryText);
        Transaction::commit();
        QVERIFY(query.isActive());
        QCOMPARE(query.size(), 1);
    }
};


QTEST_MAIN(DatabaseTest)
#include "DatabaseTest.moc"
