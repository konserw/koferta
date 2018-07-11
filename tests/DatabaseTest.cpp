#include <QtTest/QtTest>
#include "Database.hpp"
#include "DatabaseHelpers.hpp"
#include "TermItem.hpp"
#include "Customer.hpp"

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
    int test_customer_id;
    QSqlQuery customerQuery() const
    {
        QString queryText = R"(
SELECT `customersView`.`customerID`,
    `customersView`.`short`,
    `customersView`.`full`,
    `customersView`.`title`,
    `customersView`.`name`,
    `customersView`.`surname`,
    `customersView`.`address`
FROM `kOferta_test`.`customersView`;
)";
        return Transaction::run(queryText);
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
    /*
     * CUSTOMER
     */
    void saveCustomer()
    {
        auto customer = Customer("Short company name", "Long company name", "Mr.", "Jon", "Smith", "Address");
        Database::instance()->saveCustomer(customer);

        auto query = customerQuery();
        QVERIFY(query.isActive());
        QCOMPARE(query.size(), 1);
        query.next();
        test_customer_id = query.value("customerID").toInt();
        QCOMPARE(query.value("short").toString(), customer.getShortName());
        QCOMPARE(query.value("full").toString(), customer.getFullName());
        QCOMPARE(query.value("title").toString(), customer.getTitle());
        QCOMPARE(query.value("name").toString(), customer.getName());
        QCOMPARE(query.value("surname").toString(), customer.getSurname());
        QCOMPARE(query.value("address").toString(), customer.getAddress());
    }
    void editCustomer()
    {
        auto customer = Customer("diff company name", "different company name", "Ms.", "Jane", "Doe", "Different Address", test_customer_id);
        Database::instance()->editCustomer(customer);

        auto query = customerQuery();
        QVERIFY(query.isActive());
        QCOMPARE(query.size(), 1);
        query.next();
        QCOMPARE(query.value("customerID").toInt(), test_customer_id);
        QCOMPARE(query.value("short").toString(), customer.getShortName());
        QCOMPARE(query.value("full").toString(), customer.getFullName());
        QCOMPARE(query.value("title").toString(), customer.getTitle());
        QCOMPARE(query.value("name").toString(), customer.getName());
        QCOMPARE(query.value("surname").toString(), customer.getSurname());
        QCOMPARE(query.value("address").toString(), customer.getAddress());
    }
    void deleteCustomer()
    {
        Database::instance()->deleteCustomer(Customer(test_customer_id));
        auto query = customerQuery();
        QVERIFY(query.isActive());
        QCOMPARE(query.size(), 0);
    }
};


QTEST_MAIN(DatabaseTest)
#include "DatabaseTest.moc"
