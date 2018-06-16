#include <gtest/gtest.h>
#include "../MainWindow/Database.h"

void connect()
{
    QString db_name = QProcessEnvironment::systemEnvironment().value("DATABASE_MYSQL_USERNAME", "test_user");
    QString db_pass = QProcessEnvironment::systemEnvironment().value("DATABASE_MYSQL_PASSWORD", "test_password");
    Database::instance()->setupDatabaseConnection("localhost", 3306, "kOferta_test", db_name, db_pass);
    ASSERT_TRUE(Database::instance()->isConnected());
}

TEST(database, setup_connection)
{
    connect();
    Database::instance()->dropConection();
}
