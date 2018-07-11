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
    void sampleTermData()
    {
        QTest::addColumn<TermType>("term_type");
        QTest::addColumn<QString>("table");
        QTest::addColumn<QString>("short_desc");
        QTest::addColumn<QString>("long_desc");

        QTest::newRow("billing") << TermType::billing << "billingTerms" << "billing short" << "billing long";
        QTest::newRow("delivery") << TermType::delivery << "deliveryTerms" << "delivery short" << "delivery long";
        QTest::newRow("delivery date") << TermType::deliveryDate << "deliveryDateTerms" << "delivery date short" << "delivery date long";
        QTest::newRow("offer") << TermType::offer << "offerTerms" << "offer short" << "offer long";
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
    /*
     * TERMS
     */
    void createTerm_data()
    {
        sampleTermData();
    }
    void createTerm()
    {
        QFETCH(TermType, term_type);
        QFETCH(QString, table);
        QFETCH(QString, short_desc);
        QFETCH(QString, long_desc);
        TermItem testTerm(term_type, short_desc, long_desc);
        Database::createTerm(testTerm);

        QString queryText;
        queryText = QString("SELECT shortDesc, longDesc FROM %1")
                .arg(table);
        auto query = Transaction::run(queryText);

        QVERIFY(query.isActive());
        QCOMPARE(query.size(), 1);
        query.next();
        QCOMPARE(query.value("shortDesc").toString(), short_desc);
        QCOMPARE(query.value("longDesc").toString(), long_desc);
    }
    void getTerm_data()
    {
        sampleTermData();
    }
    void getTerm()
    {
        QFETCH(TermType, term_type);
        QFETCH(QString, short_desc);
        QFETCH(QString, long_desc);

        TermItem testItem = Database::getTerm(term_type, 1);
        QCOMPARE(testItem.getType(), term_type);
        QCOMPARE(testItem.shortDesc(), short_desc);
        QCOMPARE(testItem.longDesc(), long_desc);
    }
    void getTermModel_data()
    {
        sampleTermData();
    }
    void getTermModel()
    {
        QFETCH(TermType, term_type);
        QFETCH(QString, short_desc);
        QFETCH(QString, long_desc);

        std::unique_ptr<TermModel> testModel;
        testModel.reset(Database::getTermModel(term_type));
        QCOMPARE(testModel->rowCount(QModelIndex()), 1);
        QCOMPARE(testModel->data(testModel->index(0, 1, QModelIndex())).toString(), short_desc);
        QCOMPARE(testModel->data(testModel->index(0, 2, QModelIndex())).toString(), long_desc);
    }
};


QTEST_MAIN(DatabaseTest)
#include "DatabaseTest.moc"
