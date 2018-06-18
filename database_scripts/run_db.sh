#!/bin/bash
echo "Database setup script start"
 
MARIADB_USER=user
MARIADB_DATABASE=kOferta_test

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
EOF

if [[ $MARIADB_DATABASE != "" ]]; then
  echo "Creating $MARIADB_DATABASE database"
  echo "CREATE DATABASE IF NOT EXISTS \`$MARIADB_DATABASE\` CHARACTER SET utf8 COLLATE utf8_general_ci;" >> $config
  if [[ $MARIADB_USER != "" ]]; then
    echo "Creating $MARIADB_USER database user"
    echo "CREATE USER '$MARIADB_USER'@'%' IDENTIFIED WITH unix_socket;">> $config
    echo "GRANT ALL ON \`$MARIADB_DATABASE\`.* to '$MARIADB_USER'@'%';" >> $config
#    echo "FLUSH PRIVILEGES;" >> $config
  fi
fi

cat ./database_scripts/test_database_ddl.sql >> $config
/usr/sbin/mysqld --bootstrap --verbose=0 < $config
rm -f $config

#starting db service
service mysql start
echo "Database setup script finished"
