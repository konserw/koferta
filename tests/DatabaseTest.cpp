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
            Database::setupDatabaseConnection("localhost", 3306, db_name);
        else
            Database::setupDatabaseConnection("localhost", 3306, db_name, db_user, db_pass);
    }
    void sampleUserData()
    {
        QTest::addColumn<int>("uid");
        QTest::addColumn<QString>("name");
        QTest::addColumn<QString>("mail");
        QTest::addColumn<int>("male");
        QTest::addColumn<QString>("phone");
        QTest::addColumn<int>("currentOfferNumber");
        QTest::addColumn<QDate>("currentOfferNumberDate");
        QTest::addColumn<QString>("charForOfferSymbol");
        QTest::addColumn<QString>("password");
        QTest::addColumn<int>("resetPassword");

        QTest::newRow("John") << 1 << "John Smith" << "john@mail.com" << 1 << "123 456 789" << 5 << QDate(2018, 1, 1) << "S" << "JohnsSecretPassword" << 1;
        QTest::newRow("Jane") << 2 << "Jane Doe" << "jane@mail.com" << 0 << "600 100 200" << 5 << QDate::currentDate() << "D" << "MKO)_PL<>:{+" << 1;
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
        QVERIFY(Database::isConnected());
    }
    void cleanupTestCase()
    {
        Database::dropConection();
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
        Database::saveCustomer(customer);

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
        Database::editCustomer(customer);

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
        Database::deleteCustomer(Customer(test_customer_id));
        auto query = customerQuery();
        QVERIFY(query.isActive());
        QCOMPARE(query.size(), 0);
    }
    /*
     * USER
     */
    void userList_data() { sampleUserData(); }
    void userList()
    {
        QFETCH(int, uid);
        QFETCH(QString, name);
        QFETCH(QString, mail);
        QFETCH(int, male);
        QFETCH(QString, phone);
        QFETCH(int, currentOfferNumber);
        QFETCH(QDate, currentOfferNumberDate);
        QFETCH(QString, charForOfferSymbol);
        QString queryText = R"(
INSERT INTO `kOferta_test`.`users`
(`id`,`name`,`mail`,`male`,`phone`,`currentOfferNumber`,`currentOfferNumberDate`,`charForOfferSymbol`,`password`,`salt`,`resetPassword`)
VALUES
(NULL,'%1','%2',%3,'%4',%5,'%6','%7','x','x',1);
)";
        Transaction::run(queryText
                         .arg(name)
                         .arg(mail)
                         .arg(male)
                         .arg(phone)
                         .arg(currentOfferNumber)
                         .arg(currentOfferNumberDate.toString("yyyy-MM-dd"))
                         .arg(charForOfferSymbol)
                         );

        QHash<QString, int> test = Database::usersList();
        QCOMPARE(test.key(uid), name);
        QCOMPARE(test.value(name), uid);
    }
    void setPassword_data() { sampleUserData(); }
    void setPassword()
    {
        QFETCH(int, uid);
        QFETCH(QString, password);
        Database::setPassword(uid, password);

        QString queryText = R"(
SELECT
    `users`.`id`,
    `users`.`password`,
    `users`.`salt`,
    `users`.`resetPassword`
FROM `kOferta_test`.`users`;
)";
        auto query = Transaction::run(queryText);
        QVERIFY(query.isActive());
        query.next();
        while(query.value("id") < uid)
            QVERIFY(query.next());

        QCOMPARE(query.value("resetPassword").toInt(), 0);
        auto salt = query.value("salt").toString();
        QVERIFY(salt != 'x');
        auto salted_password_from_db = query.value("password").toString();
        QVERIFY(salted_password_from_db != 'x');
        auto salted_password = saltPassword(salt, password);
        QCOMPARE(salted_password_from_db, salted_password);
    }
    void logIn_data() { sampleUserData(); }
    void logIn()
    {
        QFETCH(int, uid);
        QFETCH(QString, name);
        QFETCH(QString, mail);
        QFETCH(int, male);
        QFETCH(QString, phone);
        QFETCH(QString, charForOfferSymbol);
        QFETCH(QString, password);

        User test_user = Database::getUserData(uid, password);
        QCOMPARE(test_user.getUid(), uid);
        QCOMPARE(test_user.getName(), name);
        QCOMPARE(test_user.getMail(), mail);
        QCOMPARE(test_user.isMale(), male == 1);
        QCOMPARE(test_user.getPhone(), phone);
        QCOMPARE(test_user.getCharForOfferSymbol(), charForOfferSymbol);
    }
    void logIn_invalid_pass_data() { sampleUserData(); }
    void logIn_invalid_pass()
    {
        QFETCH(int, uid);
        User test_user = Database::getUserData(uid, "invalid");
        QVERIFY(!test_user.isValid());
    }
    void logIn_invalid_uid()
    {
        User test_user = Database::getUserData(44, "invalid");
        QVERIFY(!test_user.isValid());
    }
    void getNewOfferSymbolForUser_data() { sampleUserData(); }
    void getNewOfferSymbolForUser()
    {
        QFETCH(int, uid);
        QFETCH(QString, name);
        QFETCH(QString, mail);
        QFETCH(int, male);
        QFETCH(QString, phone);
        QFETCH(int, currentOfferNumber);
        QFETCH(QDate, currentOfferNumberDate);
        QFETCH(QString, charForOfferSymbol);
        auto user = User(uid, name, phone, mail, charForOfferSymbol, male == 1, false);
        auto date = QDate::currentDate().toString("yyMM");
        auto db_date = currentOfferNumberDate.toString("yyMM");
        if(db_date != date)
            currentOfferNumber = 0;
        currentOfferNumber++;
        auto symbol = QString("I%1%2%3")
                .arg(date)
                .arg(charForOfferSymbol)
                .arg(QString::number(currentOfferNumber).rightJustified(2, '0'));

        QString test_symbol = Database::getNewOfferSymbolForUser(user);
        QCOMPARE(test_symbol, symbol);
    }
    /*
     * Others
     */
    void mainAddress_fail()
    {
        QString test_address = Database::mainAddress();
        QVERIFY(test_address.isNull());
    }
    void mainAddress()
    {
        QString address = "Sample address\nWith two lines";
        QString queryText = R"(
INSERT INTO `kOferta_test`.`addresses`
(`id`, `address`)
VALUES
(2, "%1")
)";
        Transaction::run(queryText.arg(address));

        QString test_address = Database::mainAddress();
        QCOMPARE(test_address, address);
    }
};


QTEST_MAIN(DatabaseTest)
#include "DatabaseTest.moc"
