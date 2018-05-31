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
    -DDISABLE_UNIT_TESTS=ON \
    ${VALGRIND_TESTS:+"-DVALGRIND_TESTS=${VALGRIND_TESTS}"} \
    ..
cmake --build build

#build/kofertaSteps >/dev/null &
#cucumber features 
