#!/bin/sh
set -ev

CTEST_OUTPUT_ON_FAILURE=ON
export CTEST_OUTPUT_ON_FAILURE

cmake -E make_directory build
cmake -E chdir build cmake \
    -DKOFERTA_VERSION="${BRANCH_NAME}-${SEMAPHORE_BUILD_NUMBER}" \
    -GNinja \
    -DKOFERTA_SQL_USER="${KOFERTA_SQL_USER}" \
    -DKOFERTA_SQL_PWD="${KOFERTA_SQL_PWD}" \
    -DUNIT_TESTS=on \
    ${VALGRIND_TESTS:+"-DVALGRIND_TESTS=${VALGRIND_TESTS}"} \
    ..
cmake --build build

#prepare test sql database
mysql -u $DATABASE_MYSQL_USERNAME -p$DATABASE_MYSQL_PASSWORD < database/test_database_ddl.sql
#and run tests
cmake --build build --target test

#build/kofertaSteps >/dev/null &
#cucumber features 
