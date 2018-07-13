#!/bin/bash
echo "Database setup script start"
 
MARIADB_DATABASE=kOferta_test
echo "-> using '$MARIADB_DATABASE' as database name"
MARIADB_USER=user
echo "-> using '$MARIADB_USER' as database user"

config=`mktemp`
if [[ ! -f "$config" ]]; then
    return 1
fi

if [ ! -f /var/lib/mysql/ibdata1 ]; then
  mysql_install_db
fi

cat << EOF > $config
USE mysql;
FLUSH PRIVILEGES;
GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost' WITH GRANT OPTION;
CREATE DATABASE IF NOT EXISTS \`$MARIADB_DATABASE\` CHARACTER SET utf8 COLLATE utf8_general_ci;
CREATE USER '$MARIADB_USER'@'%' IDENTIFIED WITH unix_socket;
GRANT ALL ON \`$MARIADB_DATABASE\`.* to '$MARIADB_USER'@'%';
EOF


/usr/sbin/mysqld --bootstrap --verbose=0 < $config
rm -f $config

echo "-> starting db service"
service mysql start
echo "-> creating test database structure"
mysql < ./database_scripts/test_database_ddl.sql 

echo "Database setup script finished"
