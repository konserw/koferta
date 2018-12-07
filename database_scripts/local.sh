#!/bin/sh
set -x
systemctl is-active --quiet mariadb
if [ $? -ne 0 ]; then
    sudo systemctl start mariadb
fi
mysql -e "drop database kOferta_test;"
mysql < ./database_scripts/test_database_ddl.sql
